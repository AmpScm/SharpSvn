// $Id: SvnBase.cpp 346 2008-02-22 12:48:18Z bhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

#include "svn_dso.h"
#include "svn_utf.h"
#define SHARPSVN_NO_ABORT
#include <libintl.h>

using namespace SharpSvn;

[Serializable]
ref class SvnThreadAbortException : InvalidOperationException
{
public:
	SvnThreadAbortException()
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
	if(!Environment::UserInteractive)
		return;

	AprPool pool(SvnBase::SmallThreadPool); // Let's hope this still works.. Most abort()s in subversion are not as fatal as this

	const char* pTitle = pool.AllocString(SharpSvnStrings::FatalExceptionInSubversionApi);
	const char* pText = pool.AllocString(String::Format(SharpSvnStrings::PleaseRestartThisApplicationBeforeContinuing, Environment::StackTrace));

	MessageBoxA(nullptr, pText, pTitle, MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);

	throw gcnew SvnThreadAbortException();
}


#pragma unmanaged
static bool SetHandler()
{
	sharpsvn_abort_t* handler = &sharpsvn_abort_handler;

	return (InterlockedExchangePointer((void**)&sharpsvn_abort, (void*)handler) != handler);
}
#pragma managed
void SvnBase::InstallAbortHandler()
{
	SetHandler();
}