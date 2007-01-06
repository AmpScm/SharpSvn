#include "stdafx.h"

#include "SvnAll.h"
#include "SvnObjBaton.h"

using namespace TurtleSvn;
using namespace TurtleSvn::Apr;


SvnClient::SvnClient()
	: _pool(gcnew AprPool()), SvnClientContext(_pool)
{
	_clientBatton = gcnew AprBaton<SvnClient^>(this);
	Initialize();
}

SvnClient::SvnClient(AprPool^ pool)
	: _pool(gcnew AprPool(pool)), SvnClientContext(_pool)
{
	_clientBatton = gcnew AprBaton<SvnClient^>(this);
	Initialize();
}

SvnClient::~SvnClient()
{
	AprPool^ pool = _pool;
	if(pool)
	{
		_pool = nullptr;

		if(!pool->IsDisposed)
			delete pool;
	}
}

void SvnClient::Initialize()
{
	void* baton = (void*)_clientBatton->Handle;

	CtxHandle->cancel_baton = baton;
	CtxHandle->cancel_func = &SvnClientCallBacks::svn_cancel_func;
	CtxHandle->log_msg_baton2 = baton;
	CtxHandle->log_msg_func2 = &SvnClientCallBacks::svn_client_get_commit_log2;
	CtxHandle->notify_baton2 = baton;
	CtxHandle->notify_func2 = &SvnClientCallBacks::svn_wc_notify_func2;
	CtxHandle->progress_baton = baton;
	CtxHandle->progress_func = &SvnClientCallBacks::svn_ra_progress_notify_func;
}

void SvnClient::HandleClientCancel(SvnClientCancelEventArgs^ e)
{
	if(_currentArgs)
		_currentArgs->OnCancel(e);

	if(e->Cancel)
		return;

	OnClientCancel(e);
}

void SvnClient::OnClientCancel(SvnClientCancelEventArgs^ e)
{
	if(_clientCancel)
		_clientCancel(this, e);
}

void SvnClient::HandleClientProgress(SvnClientProgressEventArgs^ e)
{
	if(_currentArgs)
		_currentArgs->OnProgress(e);

	OnClientProgress(e);
}

void SvnClient::OnClientProgress(SvnClientProgressEventArgs^ e)
{
	if(_clientProgress)
		_clientProgress(this, e);
}

void SvnClient::HandleClientGetCommitLog(SvnClientCommitLogEventArgs^ e)
{
	SvnCommitArgs^ commitArgs = dynamic_cast<SvnCommitArgs^>(_currentArgs); // C#: _currentArgs as SvnCommitArgs

	if(commitArgs)
		commitArgs->OnGetCommitLog(e);
	
	OnClientGetCommitLog(e);
}

void SvnClient::OnClientGetCommitLog(SvnClientCommitLogEventArgs^ e)
{
	
}

void SvnClient::HandleClientNotify(SvnClientNotifyEventArgs^ e)
{
	if(_currentArgs)
		_currentArgs->OnNotify(e);

	OnClientNotify(e);
}

void SvnClient::OnClientNotify(SvnClientNotifyEventArgs^ e)
{
	if(_clientNotify)
		_clientNotify(this, e);
}

svn_error_t* SvnClientCallBacks::svn_cancel_func(void *cancel_baton)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)cancel_baton);

	SvnClientCancelEventArgs^ ea = gcnew SvnClientCancelEventArgs();
	try
	{
		client->HandleClientCancel(ea);

		if(ea->Cancel)
			return svn_error_create (SVN_ERR_CANCELLED, NULL, "Operation canceled");

		return nullptr;
	}
	finally
	{
		ea->Detach(false);
	}
}

svn_error_t* SvnClientCallBacks::svn_client_get_commit_log2(const char **log_msg, const char **tmp_file, const apr_array_header_t *commit_items, void *baton, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnClientCommitLogEventArgs^ ea = gcnew SvnClientCommitLogEventArgs();

	try
	{
		client->HandleClientGetCommitLog(ea);

		if(ea->Cancel)
			return svn_error_create (SVN_ERR_CANCELLED, NULL, "Operation canceled");

		return nullptr;
	}
	finally
	{
		ea->Detach(false);
	}
}


void SvnClientCallBacks::svn_wc_notify_func2(void *baton, const svn_wc_notify_t *notify, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnClientNotifyEventArgs^ ea = gcnew SvnClientNotifyEventArgs(notify, pool);

	try
	{
		client->HandleClientNotify(ea);
	}
	finally
	{
		ea->Detach(false);
	}
}

void SvnClientCallBacks::svn_ra_progress_notify_func(apr_off_t progress, apr_off_t total, void *baton, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnClientProgressEventArgs^ ea = gcnew SvnClientProgressEventArgs(progress, total, pool);

	try
	{
		client->HandleClientProgress(ea);
	}
	finally
	{
		ea->Detach(false);
	}
}