// Copyright 2007-2008 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "stdafx.h"
#include "DelayImp.h"

#include "db.h"
#include "svn_dso.h"
#include "svn_utf.h"
#define SHARPSVN_NO_ABORT
#include <libintl.h>

using namespace SharpSvn;
using System::IO::Path;

[Serializable]
ref class SvnThreadAbortException : InvalidOperationException
{
public:
    SvnThreadAbortException()
    {
    }

    SvnThreadAbortException(String^ message)
        : InvalidOperationException(message)
    {
    }
protected:
    SvnThreadAbortException(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context)
        : InvalidOperationException(info, context)
    {
    }
};

/*
 * MessageBox() Flags
 */
#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L

#define MB_ICONHAND                 0x00000010L
#define MB_ICONQUESTION             0x00000020L
#define MB_ICONEXCLAMATION          0x00000030L
#define MB_ICONASTERISK             0x00000040L

#define MB_USERICON                 0x00000080L
#define MB_ICONWARNING              MB_ICONEXCLAMATION
#define MB_ICONERROR                MB_ICONHAND

#define MB_ICONINFORMATION          MB_ICONASTERISK
#define MB_ICONSTOP                 MB_ICONHAND

#define MB_APPLMODAL                0x00000000L
#define MB_SYSTEMMODAL              0x00001000L
#define MB_TASKMODAL                0x00002000L

extern "C" {
WINUSERAPI
int
WINAPI
MessageBoxA(
    __in_opt HWND hWnd,
    __in_opt LPCSTR lpText,
    __in_opt LPCSTR lpCaption,
    __in UINT uType);
}

void __cdecl sharpsvn_abort_handler()
{
#ifdef _DEBUG
    if (Environment::UserInteractive)
    {
        AprPool pool(SvnBase::SmallThreadPool); // Let's hope this still works.. Most abort()s in subversion are not as fatal as this

        const char* pTitle = pool.AllocString(SharpSvnStrings::FatalExceptionInSubversionApi);
        const char* pText = pool.AllocString(String::Format(SharpSvnStrings::PleaseRestartThisApplicationBeforeContinuing, Environment::StackTrace));

        MessageBoxA(nullptr, pText, pTitle, MB_OK | MB_ICONERROR | MB_TASKMODAL);
    }
#endif

    throw gcnew SvnThreadAbortException("Subversion raised an abort()\r\nPlease restart your application!");
}

static bool s_loaded = false, s_checked = false;

static sharpsvn_sharpsvn_check_bdb_availability_t sharpsvn_check_bdb;
static svn_error_t* __cdecl sharpsvn_check_bdb()
{
    if (s_loaded)
        return NULL;

    if (!s_checked)
    {
        s_checked = true;

        try
        {
            int major, minor;
            db_version(&major, &minor, NULL);
            s_loaded = true;
        }
        catch(...)
        {}
    }


    if (!s_loaded)
        return svn_error_create(SVN_ERR_FS_UNKNOWN_FS_TYPE, nullptr, "Subversion filesystem driver for Berkeley DB (SharpSvn-DB44-20-" APR_STRINGIFY(SHARPSVN_PLATFORM_SUFFIX) ".dll) is not installed. Can't access this repository kind.");

    return NULL;
}

static svn_error_t* __cdecl
sharpsvn_malfunction_handler(svn_boolean_t can_return, const char *file, int line, const char *expr)
{
    UNUSED_ALWAYS(can_return);
    throw gcnew SvnMalfunctionException(SvnBase::Utf8_PtrToString(expr), SvnBase::Utf8_PtrToString(file), line);
}

FARPROC WINAPI SharpSvnDelayLoadFailure(unsigned dliNotify, PDelayLoadInfo pdli)
{
    if (dliNotify != dliFailLoadLib)
        return nullptr; // Nothing we can fix

#ifdef _DEBUG
    ::OutputDebugStringA("Automatic delay loading one of the SharpSvn helper DLLs failed; trying to work around\n");
#endif

    Uri^ codeBase;

    if (!Uri::TryCreate(SvnBase::typeid->Assembly->CodeBase, UriKind::Absolute, codeBase))
        return nullptr; // We don't know where we were loaded from

    String^ path = SvnTools::PathCombine(Path::GetDirectoryName(SvnTools::GetNormalizedFullPath(codeBase->LocalPath)), gcnew String(pdli->szDll));

    pin_ptr<const wchar_t> pChars = PtrToStringChars(path);


    HMODULE hMod = ::LoadLibraryW(pChars);

    return (FARPROC)hMod;
}

#pragma unmanaged
static bool SetHandler()
{
    sharpsvn_abort_t* handler = &sharpsvn_abort_handler;

    InterlockedExchangePointer((void**)&sharpsvn_sharpsvn_check_bdb_availability, (void*)sharpsvn_check_bdb);
    InterlockedExchangePointer((void**)&__pfnDliFailureHook2, (void*)SharpSvnDelayLoadFailure);

    svn_error_set_malfunction_handler(sharpsvn_malfunction_handler);

    return (InterlockedExchangePointer((void**)&sharpsvn_abort, (void*)handler) != handler);
}

#pragma managed




void SvnBase::InstallAbortHandler()
{
    SetHandler();


}
