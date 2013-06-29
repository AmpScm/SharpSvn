// Copyright 2008-2009 The SharpSvn Project
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
#include "SvnAll.h"
#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types


using namespace SharpSvn;
using namespace SharpSvn::Implementation;

SvnWorkingCopyClient::SvnWorkingCopyClient()
: _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
	_clientBaton = gcnew AprBaton<SvnWorkingCopyClient^>(this);

	Initialize();
}

SvnWorkingCopyClient::~SvnWorkingCopyClient()
{
	delete _clientBaton;
}

struct SvnWcClientCallBacks
{
	static svn_error_t * __cdecl svn_cancel_func(void *cancel_baton);
	static void __cdecl svn_wc_notify_func2(void *baton, const svn_wc_notify_t *notify, apr_pool_t *pool);
};

void SvnWorkingCopyClient::Initialize()
{
	void* baton = (void*)_clientBaton->Handle;

	CtxHandle->cancel_baton = baton;
	CtxHandle->cancel_func = &SvnWcClientCallBacks::svn_cancel_func;
	CtxHandle->notify_baton2 = baton;
	CtxHandle->notify_func2 = &SvnWcClientCallBacks::svn_wc_notify_func2;
}

svn_error_t* SvnWcClientCallBacks::svn_cancel_func(void *cancel_baton)
{
	SvnWorkingCopyClient^ client = AprBaton<SvnWorkingCopyClient^>::Get((IntPtr)cancel_baton);

	SvnCancelEventArgs^ ea = gcnew SvnCancelEventArgs();
	try
	{
		client->HandleClientCancel(ea);

		if (ea->Cancel)
			return svn_error_create (SVN_ERR_CANCELLED, nullptr, "Operation canceled from OnCancel");

		return nullptr;
	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("Cancel function", e);
	}
	finally
	{
		ea->Detach(false);
	}
}

void SvnWorkingCopyClient::HandleClientCancel(SvnCancelEventArgs^ e)
{
	if (CurrentCommandArgs)
		CurrentCommandArgs->RaiseOnCancel(e);

	if (e->Cancel)
		return;

	OnCancel(e);
}

void SvnWorkingCopyClient::OnCancel(SvnCancelEventArgs^ e)
{
	Cancel(this, e);
}

void SvnWcClientCallBacks::svn_wc_notify_func2(void *baton, const svn_wc_notify_t *notify, apr_pool_t *pool)
{
	SvnWorkingCopyClient^ client = AprBaton<SvnWorkingCopyClient^>::Get((IntPtr)baton);
	AprPool aprPool(pool, false);

	SvnNotifyEventArgs^ ea = gcnew SvnNotifyEventArgs(notify, client->CurrentCommandArgs->CommandType, %aprPool);

	try
	{
		client->HandleClientNotify(ea);
	}
	finally
	{
		ea->Detach(false);
	}
}

void SvnWorkingCopyClient::HandleClientNotify(SvnNotifyEventArgs^ e)
{
	if (CurrentCommandArgs)
		CurrentCommandArgs->RaiseOnNotify(e);

	OnNotify(e);
}

void SvnWorkingCopyClient::OnNotify(SvnNotifyEventArgs^ e)
{
	Notify(this, e);
}

void SvnWorkingCopyClient::HandleProcessing(SvnProcessingEventArgs^ e)
{
	OnProcessing(e);
}

void SvnWorkingCopyClient::OnProcessing(SvnProcessingEventArgs^ e)
{
	Processing(this, e);
}

void SvnWorkingCopyClient::HandleClientError(SvnErrorEventArgs^ e)
{
	if (e->Cancel)
		return;

	if (CurrentCommandArgs)
	{
		CurrentCommandArgs->RaiseOnSvnError(e);

		if (e->Cancel)
			return;
	}

	OnSvnError(e);
}

void SvnWorkingCopyClient::OnSvnError(SvnErrorEventArgs^ e)
{
	SvnError(this, e);
}
