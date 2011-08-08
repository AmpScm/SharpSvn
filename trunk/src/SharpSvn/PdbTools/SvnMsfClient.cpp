#include "stdafx.h"
#ifndef NDEBUG
#include "PdbTools/SvnMsfClient.h"

#include "sbs_pdh.h"

using namespace System::Collections::Generic;
using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using System::IO::Stream;

SvnMsfClient::SvnMsfClient()
{
}

void SvnMsfClient::Release()
{
    if (_data)
    {
        void *p = _data;
        UnmapViewOfFile(p);
    }

    if (_mapping)
    {
        HANDLE h = _mapping;
        _mapping = nullptr;
        CloseHandle(h);
    }

    if (_file)
    {
        HANDLE h = _file;
        _file = nullptr;
        CloseHandle(h);
    }
}

SvnMsfClient::~SvnMsfClient()
{
    Release();
}

SvnMsfClient::!SvnMsfClient()
{
    Release();
}

bool SvnMsfClient::Open(String^ path)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");
    else if (_data || _mapping || _file)
        throw gcnew InvalidOperationException();

    pin_ptr<const wchar_t> pPath = PtrToStringChars(path);
    _file = CreateFileW(pPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (_file == INVALID_HANDLE_VALUE)
    {
        int n = GetLastError();
        throw gcnew SystemException(String::Format("Can't open file: {0}", n));
    }

    _size = GetFileSize(_file, NULL);

    if (_size == INVALID_FILE_SIZE)
        throw gcnew SystemException("File too large");

    _mapping = CreateFileMappingW(_file, NULL, PAGE_READWRITE, 0, 0, NULL);

    if (!_mapping)
        throw gcnew SystemException("Can't create mapping");

    _data = (Byte*)MapViewOfFile(_mapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

    Exception ^e;
    if ((e = VerifyFile()))
    {
        Release();
        throw e;
    }

    return true;
}

int SvnMsfClient::PagesRequired(int size)
{
    if (!_data)
        return -1;

    if (size == 0)
        return 0;

    PDB_HEADER *hdr = (PDB_HEADER*)_data;

    return ((size - 1) / hdr->V700.dPageBytes)+1;
}

static DWORD ReadDWORD(Stream^ stream)
{
    array<Byte>^ dwordArray = gcnew array<Byte>(4);

    int n = stream->Read(dwordArray, 0, 4);
    if (n != 4)
        throw gcnew InvalidOperationException();

    return (DWORD)BitConverter::ToInt32(dwordArray, 0);
}

static bool BitSet(BYTE* bytes, DWORD bit)
{
    BYTE byte = bytes[bit / 8];
    return ((byte & (1 << (bit % 8))) != 0);
}

/*static bool BitSet(array<Byte>^ bytes, DWORD bit)
{
    if (bit < 0 || bit / 8 > dwords->Length)
        return false;
    BYTE byte = bytes[bit / 8];
    return ((byte & (1 << (bit % 8))) != 0);
}*/

static bool BitSet(array<DWORD>^ dwords, int bit)
{
    if (bit < 0 || bit / 32 >= dwords->Length)
        return false;
    DWORD dword = dwords[bit / 32];
    return ((dword & (1 << (bit % 32))) != 0);
}

static String^ GetString(array<Byte>^ bytes, int start)
{
    int end = start;
    while (end < bytes->Length && 0 != bytes[end])
        end++;

    return System::Text::Encoding::UTF8->GetString(bytes, start, end-start);
}

Exception^ SvnMsfClient::VerifyFile()
{
    if (_size < sizeof(PDB_HEADER))
        return gcnew InvalidOperationException("File too small");

    Byte *pData = (Byte*)_data;
    PDB_HEADER_700 *hdr = (PDB_HEADER_700*)pData;

    if (memcmp(hdr->abSignature, PDB_SIGNATURE_700, PDB_SIGNATURE_700_))
        return gcnew SystemException("Unexpected file format; only MSF format 7.0 is supported atg this time");

    _pageBytes = hdr->dPageBytes;
    _flagPage = hdr->dFlagPage;
    _filePages = hdr->dFilePages;
    _rootBytes = hdr->dRootBytes;

    if (_filePages * _pageBytes != (int)_size)
        return gcnew SystemException("Incorrect file size");

    array<bool>^ usedPages = gcnew array<bool>(_filePages);
    array<DWORD>^ rootPages = gcnew array<DWORD>(PagesRequired(_rootBytes));
    {
        int indexBytes = rootPages->Length * sizeof(DWORD);

        array<DWORD>^ indexPages = gcnew array<DWORD>(PagesRequired(indexBytes));

        for (int i = 0; i < indexPages->Length; i++)
        {
            indexPages[i] = hdr->adIndexPages[i];
            usedPages[indexPages[i]] = true;
        }

        SvnMsfStream indexStream(this, indexPages, indexBytes);

        for (int i = 0; i < rootPages->Length; i++)
        {
            rootPages[i] = ReadDWORD(%indexStream);
            usedPages[rootPages[i]] = true;
        }
    }

    SvnMsfStream rootStream(this, rootPages, _rootBytes);

    _nStreams = ReadDWORD(%rootStream);
    DWORD nTotalPages = 0;
    array<array<DWORD>^>^ streams = gcnew array<array<DWORD>^>(_nStreams);
    array<DWORD>^ streamSizes = gcnew array<DWORD>(_nStreams);

    for (int n = 0; n < _nStreams; n++)
    {
        DWORD nSize = ReadDWORD(%rootStream);
        streamSizes[n] = nSize;

        int nPages = PagesRequired(nSize);
        streams[n] = gcnew array<DWORD>(nPages);

        nTotalPages += nPages;

        GC::KeepAlive(nSize);
    }

    for (int n = _flagPage + PagesRequired(_filePages / 8)-1; n >= 0 ; n--)
    {
        usedPages[n] = true;
    }

    for each (array<DWORD>^ s in streams)
    {
        for (int n = 0; n < s->Length; n++)
        {
            DWORD nPage = ReadDWORD(%rootStream);
            s[n] = nPage;

            usedPages[nPage] = true;
        }
    }

    BYTE *pFat = _data + _pageBytes * _flagPage;

    for (int n = 0; n < _filePages; n++)
    {
        if (BitSet(pFat, n))
        {
            if (usedPages[n])
            {
                int nn = 0;
                bool found = false;
                for each(array<DWORD>^ s in streams)
                {
                    if (Array::IndexOf(s, (DWORD)n) >= 0)
                    {
                        if (nn > 0)
                            System::Diagnostics::Debug::WriteLine(String::Format("Page {0}, in use by stream {0}, is marked free", n, nn));

                        found = true;
                        break;
                    }
                }

                if (!found)
                    System::Diagnostics::Debug::WriteLine(String::Format("Flag mismatch on page {0}", n));
            }
        }
    }

    _streams = streams;
    _streamSizes = streamSizes;

    //GC::KeepAlive((int)indexPages);
    //GC::KeepAlive((int)pRoot);

    return nullptr;
}

void SvnMsfClient::LoadNames()
{
    if (!_streams || !_data)
        throw gcnew InvalidOperationException();
    else if (_streams->Length < 2)
        return;

    SvnMsfStream nameStream(this, _streams[1], _streamSizes[1]);
    System::IO::BinaryReader bits(%nameStream);

    // Definition from the CodePlex CCI Metadata project

    int version = bits.ReadInt32();
    if (version != 20000404)
        throw gcnew InvalidOperationException("Invalid version");
    bits.ReadInt32(); // Signature
    bits.ReadInt32(); // Age
    bits.ReadBytes(16); // GUID

    int nChars = bits.ReadInt32();
    array<Byte>^ names = bits.ReadBytes(nChars);

    int cnt = bits.ReadInt32();
    int max = bits.ReadInt32();

    int nPresent = bits.ReadInt32();
    array<DWORD>^ presentBits = gcnew array<DWORD>(nPresent);
    for (int n = 0; n < nPresent; n++)
        presentBits[n] = bits.ReadInt32();

    // The deleted bitset should be all 0.
    int nDeleted = bits.ReadInt32();
    array<DWORD>^ deletedBits = gcnew array<DWORD>(nDeleted);
    for (int n = 0; n < nDeleted; n++)
        deletedBits[n] = bits.ReadInt32();

    Dictionary<String^, int>^ map = gcnew Dictionary<String^, int>();

    for (int i = 0; i < max; i++)
    {
        if (BitSet(deletedBits, i))
            throw gcnew InvalidOperationException("Can't handle deleted");
        if (BitSet(presentBits, i))
        {
            int ns = bits.ReadInt32();
            int ni = bits.ReadInt32();

            map->Add(GetString(names, ns), ni);
        }
    }

    if ((int)cnt != map->Count)
        throw gcnew InvalidOperationException("Invalid bitset?");

    bits.ReadInt32(); // 0

    System::Diagnostics::Debug::Assert(nameStream.Position == nameStream.Length);
    _streamNames = map;
}

Stream^ SvnMsfClient::GetStream(int index)
{
    if (index < 0 || index >= _nStreams)
        throw gcnew ArgumentOutOfRangeException("index");
    else if (!_streams || !_data)
        throw gcnew InvalidOperationException();

    return gcnew SvnMsfStream(this, _streams[index], _streamSizes[index]);
}

Stream^ SvnMsfClient::GetStream(String^ name)
{
    if (String::IsNullOrEmpty(name))
        throw gcnew ArgumentNullException("name");
    else if (!_streams || !_data)
        throw gcnew InvalidOperationException();

    if (!_streamNames)
    {
        LoadNames();

        if (!_streamNames)
            return nullptr;
    }

    int n;
    if (_streamNames->TryGetValue(name, n))
        return GetStream(n);

    return nullptr;
}

ICollection<String^>^ SvnMsfClient::GetStreamNames()
{
    if (!_streams || !_data)
        throw gcnew InvalidOperationException();

    if (!_streamNames)
    {
        LoadNames();

        if (!_streamNames)
            return nullptr;
    }

    return _streamNames->Keys;
}

__int64 SvnMsfClient::Read(array<DWORD>^ pages, __int64 pos, __int64 size, array<Byte>^ buffer, int index, int length)
{
    if (!_data)
        throw gcnew InvalidOperationException();
    else if (index < 0 || index > buffer->Length)
        throw gcnew ArgumentOutOfRangeException("index");
    else if (length < 0 || (__int64)index + ((__int64)length) > buffer->Length)
        throw gcnew ArgumentOutOfRangeException("length");

    if (length > size-pos)
        length = (int)(size-pos);

    while (length > 0 && pos < size)
    {
        int page = (int)(pos / _pageBytes);
        int canRead = _pageBytes - (pos % _pageBytes);

        if (page < 0 || page >= pages->Length)
            throw gcnew InvalidOperationException("Can't read outside page range");
        else if (page == pages->Length-1)
            canRead = (int)(size - (page * _pageBytes));

        int toRead = (canRead < length) ? canRead : length;

        int pageToRead = pages[page];

        if (pageToRead <= 0 || pageToRead >= _filePages)
            throw gcnew InvalidOperationException("Invalid page reference");

        pin_ptr<Byte> p = &buffer[index];
        memcpy(p, _data + (_pageBytes * pageToRead) + pos % _pageBytes, toRead);

        pos += toRead;
        index += toRead;
        length -= toRead;
    }

    return pos;
}

SvnMsfStream::SvnMsfStream(SvnMsfClient^ client, array<DWORD>^ pages, __int64 size)
{
    if (!client)
        throw gcnew ArgumentNullException("client");
    else if (!pages)
        throw gcnew ArgumentNullException("pages");
    else if (size < 0)
        throw gcnew ArgumentOutOfRangeException("size");

    _client = client;
    _pages = pages;
    _size = size;
}

int SvnMsfStream::Read(array<Byte> ^data, int start,int length)
{
    __int64 newPos = _client->Read(_pages, _pos, _size, data, start, length);
    int nRead = (int)(newPos - _pos);
    _pos = newPos;
    return nRead;
}

__int64 SvnMsfStream::Seek(__int64 offset, System::IO::SeekOrigin origin)
{
    __int64 newPos;
    switch (origin)
    {
    case System::IO::SeekOrigin::Begin:
        newPos = offset;
        break;
    case System::IO::SeekOrigin::End:
        newPos = _size - offset;
        break;
    case System::IO::SeekOrigin::Current:
        newPos = _pos + offset;
        break;
    default:
        throw gcnew ArgumentOutOfRangeException("origin");
    }

    if (newPos < 0 || newPos > _size)
        throw gcnew ArgumentOutOfRangeException("offset");

    return _pos = newPos;
}
#endif