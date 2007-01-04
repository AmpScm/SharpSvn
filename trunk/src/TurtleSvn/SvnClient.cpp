#include "stdafx.h"

#include "SvnAll.h"
#include <svn_config.h>
#include "SvnObjBaton.h"

using namespace QQn::Apr;
using namespace QQn::Svn;

struct apr_hash_t
{};


SvnClient::SvnClient()
	: _pool(gcnew AprPool()), SvnClientContext(_pool)
{
	_clientBatton = gcnew SvnBatton<SvnClient^>(this);
	Initialize();
}

SvnClient::SvnClient(AprPool^ pool)
	: _pool(gcnew AprPool(pool)), SvnClientContext(_pool)
{
	_clientBatton = gcnew SvnBatton<SvnClient^>(this);
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



void SvnClient::EnsureState(SvnClientState state)
{
	if(state < _clientState)
		return;

	if(ClientState < SvnClientState::ConfigLoaded && state >= SvnClientState::ConfigLoaded)
	{
		LoadConfigurationDefault();

		System::Diagnostics::Debug::Assert(ClientState == SvnClientState::ConfigLoaded);
	}
}

void SvnClient::LoadConfiguration(String ^path, bool ensurePath)
{
	if(ClientState >= SvnClientState::ConfigLoaded)
		throw gcnew InvalidOperationException("Configuration already loaded");

	if(String::IsNullOrEmpty(path))
		path = nullptr;

	AprPool^ tmpPool = gcnew AprPool(_pool);
	try
	{
		const char* szPath = path ? tmpPool->AllocString(path) : nullptr;

		svn_error_t* err = nullptr;

		if(ensurePath)
		{
			err = svn_config_ensure(szPath, tmpPool->Handle);

			if(err)
				throw gcnew SvnException(err);
		}

		apr_hash_t* cfg = nullptr;
		err = svn_config_get_config(&cfg, szPath, _pool->Handle);

		if(err)
			throw gcnew SvnException(err);

		CtxHandle->config = cfg;

		_clientState = SvnClientState::ConfigLoaded;
	}
	finally
	{
		delete tmpPool;
	}
}

void SvnClient::LoadConfiguration(String ^path)
{
	LoadConfiguration(path, false);
}

void SvnClient::LoadConfigurationDefault()
{
	LoadConfiguration(nullptr, false);
}

void SvnClient::MergeConfiguration(String^ path)
{
	if(!path)
		throw gcnew ArgumentNullException("path");

	if(ClientState < SvnClientState::ConfigLoaded)
		LoadConfigurationDefault();

	AprPool^ tmpPool = gcnew AprPool(_pool);
	try
	{
		const char* szPath = tmpPool->AllocString(path);

		svn_error_t* err = svn_config_get_config(&CtxHandle->config, szPath, _pool->Handle);

		if(err)
			throw gcnew SvnException(err);
	}
	finally
	{
		delete tmpPool;
	}
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
	ClientCancel(this, e);
}

void SvnClient::HandleClientProgress(SvnClientProgressEventArgs^ e)
{
	if(_currentArgs)
		_currentArgs->OnProgress(e);

	OnClientProgress(e);
}

void SvnClient::OnClientProgress(SvnClientProgressEventArgs^ e)
{
	ClientProgress(this, e);
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
	ClientGetCommitLog(this, e);
}

void SvnClient::HandleClientNotify(SvnClientNotifyEventArgs^ e)
{
	if(_currentArgs)
		_currentArgs->OnNotify(e);

	OnClientNotify(e);
}

void SvnClient::OnClientNotify(SvnClientNotifyEventArgs^ e)
{
	ClientNotify(this, e);
}

svn_error_t* SvnClientCallBacks::svn_cancel_func(void *cancel_baton)
{
	SvnClient^ client = SvnBatton<SvnClient^>::Get((IntPtr)cancel_baton);

	SvnClientCancelEventArgs^ ea = gcnew SvnClientCancelEventArgs();

	client->HandleClientCancel(ea);

	return nullptr;
}

svn_error_t* SvnClientCallBacks::svn_client_get_commit_log2(const char **log_msg, const char **tmp_file, const apr_array_header_t *commit_items, void *baton, apr_pool_t *pool)
{
	SvnClient^ client = SvnBatton<SvnClient^>::Get((IntPtr)baton);

	SvnClientCommitLogEventArgs^ ea = gcnew SvnClientCommitLogEventArgs();

	client->HandleClientGetCommitLog(ea);

	return nullptr;
}


void SvnClientCallBacks::svn_wc_notify_func2(void *baton, const svn_wc_notify_t *notify, apr_pool_t *pool)
{
	SvnClient^ client = SvnBatton<SvnClient^>::Get((IntPtr)baton);

	SvnClientNotifyEventArgs^ ea = gcnew SvnClientNotifyEventArgs();

	client->HandleClientNotify(ea);

}

void SvnClientCallBacks::svn_ra_progress_notify_func(apr_off_t progress, apr_off_t total, void *baton, apr_pool_t *pool)
{
	SvnClient^ client = SvnBatton<SvnClient^>::Get((IntPtr)baton);

	SvnClientProgressEventArgs^ ea = gcnew SvnClientProgressEventArgs(progress, total);

	client->HandleClientProgress(ea);
}