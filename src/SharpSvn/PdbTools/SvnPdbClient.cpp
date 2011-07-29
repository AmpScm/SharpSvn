
#include "stdafx.h"

#include <WinCrypt.h>
/* The DIA SDK is installed as part of Visual Studio 2005 and 2008. "../../" is the Visual Studio root dir */
#ifdef NOUSER
typedef struct tagMSG FAR *LPMSG;
#endif
#include <../../DIA SDK/include/dia2.h>

#include "PdbTools/SvnPdbClient.h"

#define SysFreeString(bstr) System::Runtime::InteropServices::Marshal::FreeBSTR((IntPtr)bstr)

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

SvnPdbClient::SvnPdbClient()
{
    IDiaDataSource *datasource;
    if (SUCCEEDED(CoCreateInstance(__uuidof(DiaSource), NULL,
        CLSCTX_INPROC_SERVER,
        __uuidof(IDiaDataSource),
        (void**)&datasource)))
        _dataSource = datasource;
}

void SvnPdbClient::Release()
{
    if (_scope != nullptr)
    {
        IDiaSymbol *scope = _scope;
        _scope = nullptr;

        scope->Release();
    }

    if (_session != nullptr)
    {
        IDiaSession *session = _session;
        _session = nullptr;

        session->Release();
    }

    if (_dataSource != nullptr)
    {
        IDiaDataSource *ds = _dataSource;
        _dataSource = nullptr;

        ds->Release();
    }
}

SvnPdbClient::~SvnPdbClient()
{
    Release();
}

SvnPdbClient::!SvnPdbClient()
{
    Release();
}

bool SvnPdbClient::Open(String^ path)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");
    else if (!Available || _session)
        throw gcnew InvalidOperationException();

    pin_ptr<const wchar_t> pPath = PtrToStringChars(path);

    if (FAILED(_dataSource->loadDataFromPdb(pPath))
        && FAILED(_dataSource->loadDataForExe(pPath, nullptr, nullptr)))
    {
        throw gcnew SystemException("Opening PDB failed");
    }

    IDiaSession *ds;

    if (FAILED(_dataSource->openSession(&ds)))
    {
        throw gcnew SystemException("Opening Session failed");
    }

    _session = ds;

    IDiaSymbol *symbol;

    if (FAILED(_session->get_globalScope(&symbol)))
    {
        throw gcnew SystemException("Opening Global scope failed");
    }
    _scope = symbol;

    return true;
}

using System::Collections::Generic::Dictionary;
using System::Collections::Generic::List;
bool SvnPdbClient::GetAllSourceFiles(System::Collections::Generic::IList<String^> ^%files)
{
    if (!Available || !_session)
        throw gcnew InvalidOperationException();

    Dictionary<String^, String^>^ allFiles = gcnew Dictionary<String^, String^>();

    IDiaEnumSymbols* pEnumCompiland = nullptr;
    IDiaSymbol* pCompiland;
    IDiaEnumSourceFiles* pEnumSrcFile;
    ULONG celt = 0;

    // In order to find the source files, we have to look at the image's compilands/modules
    if (!SUCCEEDED(_scope->findChildren(SymTagCompiland, NULL, nsNone, &pEnumCompiland)))
    {
        return false;
    }

    while(S_OK == pEnumCompiland->Next( 1, &pCompiland, &celt))
    {
        /*BSTR wszCompilandName;

        if (pCompiland->get_name(&wszCompilandName) == S_OK)
        {
        String^ name = gcnew String(wszCompilandName);
        allFiles[name] = name;

        SysFreeString(wszCompilandName);
        }*/

        // Every compiland could contain multiple references to the source files which were used to build it
        // Retrieve all source files by compiland by passing NULL for the name of the source file
        if (SUCCEEDED(_session->findFile(pCompiland, NULL, nsNone, &pEnumSrcFile)))
        {
            ULONG celt = 0;
            IDiaSourceFile* pSrcFile;

            while (S_OK == pEnumSrcFile->Next( 1, &pSrcFile, &celt ))
            {
                BSTR bName;
                DWORD checksumType = 0;

                if (!SUCCEEDED(pSrcFile->get_checksumType(&checksumType)))
                    checksumType = 0;

                if (SUCCEEDED(pSrcFile->get_fileName(&bName)) && bName)
                {
                    String^ name = gcnew String(bName);
                    String^ csValue = "";
                    SysFreeString(bName);

                    if (checksumType == CALG_MD5
                        || checksumType == CALG_SHA1)
                    {
                        csValue = "";
                    }

                    allFiles[name] = csValue;
                }
                pSrcFile->Release();
            }
            pEnumSrcFile->Release();
        }
    }
    pCompiland->Release();

    pEnumCompiland->Release();
    files = (gcnew List<String^>(allFiles->Keys))->AsReadOnly();
    return true;
}

