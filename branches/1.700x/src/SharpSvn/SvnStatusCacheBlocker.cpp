#include "stdafx.h"

#include "SvnAll.h"
#include "SvnStatusCacheBlocker.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

SvnStatusCacheBlocker::SvnStatusCacheBlocker(String^ path)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");

    _fullPath = SvnTools::GetNormalizedFullPath(path);

    _blocked = SendCommand(4 /* Block */, _fullPath);
}

SvnStatusCacheBlocker::~SvnStatusCacheBlocker()
{
    if (_blocked)
        _blocked = !SendCommand(5 /* Unblock */, _fullPath);
}

bool SvnStatusCacheBlocker::Release()
{
    return Release(_fullPath);
}

bool SvnStatusCacheBlocker::Release(String^ path)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");

    String^ fullPath = SvnTools::GetNormalizedFullPath(path);

    return SendCommand(3, fullPath);
}

bool SvnStatusCacheBlocker::Crawl(String^ path)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");

    String^ fullPath = SvnTools::GetNormalizedFullPath(path);

    return SendCommand(1, fullPath);
}

bool SvnStatusCacheBlocker::Refresh()
{
    return SendCommand(2, "");
}

static SvnStatusCacheBlocker::SvnStatusCacheBlocker()
{
    HANDLE hProcess;
    if(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hProcess))
    {
        DWORD len = 0;
        
        if (!GetTokenInformation(hProcess, TokenStatistics, NULL, 0, &len)
            && (len >= sizeof(TOKEN_STATISTICS)))
        {
            TOKEN_STATISTICS *result = (TOKEN_STATISTICS *)_alloca(len);

            if (GetTokenInformation(hProcess, TokenStatistics, result, len, &len))
            {
                LUID uid = result->AuthenticationId;
                _sessionId = (__int64)uid.HighPart << 32 | (__int64)uid.LowPart;
            }
        }
        ::CloseHandle(hProcess);
    }
}

bool SvnStatusCacheBlocker::SendCommand(Byte cmd, String^ value)
{
    if (!value)
        throw gcnew ArgumentNullException("value");
    else if (value->Length > MAX_PATH)
        return false; /* Not possible with this protocol */

    String ^sChannel = String::Format("\\\\.\\pipe\\TSVNCacheCommand-{0:X16}", WindowsLogonSessionId);
    pin_ptr<const wchar_t> pChannel = PtrToStringChars(sChannel);

    HANDLE hPipe = CreateFile(pChannel,
                              GENERIC_READ | GENERIC_WRITE,           // read and write access
                              FILE_SHARE_READ | FILE_SHARE_WRITE,     // No sharing
                              NULL,
                              OPEN_EXISTING,
                              FILE_FLAG_NO_BUFFERING,
                              NULL);
    
    if (hPipe == INVALID_HANDLE_VALUE)
        return false;

    try
    {
        DWORD dwMode = PIPE_READMODE_MESSAGE | PIPE_NOWAIT;

        if (!SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL))
            return false;

        size_t buflen = sizeof(Byte) + sizeof(wchar_t) * (MAX_PATH + 1);
        char *buffer = (char*)_alloca(buflen);
        memset(buffer, 0, buflen);
        buffer[0] = cmd;
        pin_ptr<const wchar_t> pValue = PtrToStringChars(value);
        memcpy(&buffer[1], pValue, sizeof(wchar_t) * value->Length);

        DWORD dwWritten = 0;
        if (!WriteFile(hPipe, buffer, buflen, &dwWritten, NULL))
            return false;

        memset(buffer, 0, buflen);

        if (!WriteFile(hPipe, buffer, buflen, &dwWritten, NULL))
            return false;

        return true;
    }
    finally
    {
        CloseHandle(hPipe);
    }
}