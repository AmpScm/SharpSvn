#include "stdafx.h"

#include "SvnAll.h"

using namespace SharpSvn;
using namespace SharpSvn::Apr;

svn_error_t* CreateExceptionSvnError(String^ origin, Exception^ exception)
{
	AprPool tmpPool;

	svn_error_t *innerError = nullptr;

	if(exception)
	{
		AprBaton<Exception^> ^item = gcnew AprBaton<Exception^>(exception);

		String^ forwardData = String::Format(System::Globalization::CultureInfo::InvariantCulture, MANAGED_EXCEPTION_PREFIX "{0}", (__int64)item->Handle);

		innerError = svn_error_create(SVN_ERR_CANCELLED, nullptr, tmpPool.AllocString(forwardData));
	}

	return svn_error_create(SVN_ERR_CANCELLED, innerError, tmpPool.AllocString(String::Format("Operation canceled. Exception occured in {0}", origin)));
}


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

struct SvnClientCallBacks
{
	static svn_error_t * __cdecl svn_cancel_func(void *cancel_baton);
	static svn_error_t *__cdecl svn_client_get_commit_log3(const char **log_msg, const char **tmp_file, const apr_array_header_t *commit_items, void *baton, apr_pool_t *pool);
	static void __cdecl svn_wc_notify_func2(void *baton, const svn_wc_notify_t *notify, apr_pool_t *pool);
	static void __cdecl svn_ra_progress_notify_func(apr_off_t progress, apr_off_t total, void *baton, apr_pool_t *pool);

	static svn_error_t * __cdecl svn_wc_conflict_resolver_func(svn_wc_conflict_result_t *result, const svn_wc_conflict_description_t *description, void *baton, apr_pool_t *pool);
};

void SvnClient::Initialize()
{
	void* baton = (void*)_clientBatton->Handle;

	CtxHandle->cancel_baton = baton;
	CtxHandle->cancel_func = &SvnClientCallBacks::svn_cancel_func;
	CtxHandle->log_msg_baton3 = baton;
	CtxHandle->log_msg_func3 = &SvnClientCallBacks::svn_client_get_commit_log3;
	CtxHandle->notify_baton2 = baton;
	CtxHandle->notify_func2 = &SvnClientCallBacks::svn_wc_notify_func2;
	CtxHandle->progress_baton = baton;
	CtxHandle->progress_func = &SvnClientCallBacks::svn_ra_progress_notify_func;
	CtxHandle->conflict_baton = baton;
	CtxHandle->conflict_func = &SvnClientCallBacks::svn_wc_conflict_resolver_func;
}

System::Version^ SvnClient::Version::get()
{
	const svn_version_t* version = svn_client_version();

	return gcnew System::Version(version->major, version->minor, version->patch);
}

System::Version^ SvnClient::WrapperVersion::get()
{
	return (gcnew System::Reflection::AssemblyName(SvnClient::typeid->Assembly->FullName))->Version;
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

void SvnClient::HandleClientGetCommitLog(SvnClientBeforeCommitEventArgs^ e)
{
	SvnClientArgsWithCommit^ commitArgs = dynamic_cast<SvnClientArgsWithCommit^>(CurrentArgs); // C#: _currentArgs as SvnCommitArgs

	if(commitArgs)
		commitArgs->OnBeforeCommit(e);

	OnClientBeforeCommit(e);
}

void SvnClient::OnClientBeforeCommit(SvnClientBeforeCommitEventArgs^ e)
{
	if(_clientBeforeCommit)
		_clientBeforeCommit(this, e);
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

void SvnClient::HandleClientConflictResolver(SvnClientConflictResolveEventArgs^ e)
{
	OnClientConflictResolver(e);
}

void SvnClient::OnClientConflictResolver(SvnClientConflictResolveEventArgs^ e)
{
	if(_clientConflictResolver)
		_clientConflictResolver(this, e);
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
	catch(Exception^ e)
	{
		return CreateExceptionSvnError("Cancel function", e);
	}
	finally
	{
		ea->Detach(false);
	}
}

svn_error_t* SvnClientCallBacks::svn_client_get_commit_log3(const char **log_msg, const char **tmp_file, const apr_array_header_t *commit_items, void *baton, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	AprPool^ tmpPool = gcnew AprPool(pool, false);

	SvnClientBeforeCommitEventArgs^ ea = gcnew SvnClientBeforeCommitEventArgs(commit_items, tmpPool);

	*log_msg = nullptr;
	*tmp_file = nullptr;

	try
	{
		client->HandleClientGetCommitLog(ea);

		if(ea->Cancel)
			return svn_error_create (SVN_ERR_CANCELLED, NULL, "Operation canceled");
		else if(ea->LogMessage)
			*log_msg = tmpPool->AllocString(ea->LogMessage); 
		else if(client->Configuration->LogMessageRequired)
			return svn_error_create (SVN_ERR_CANCELLED, NULL, "Commit canceled: A logmessage is required");
		else
			*log_msg = tmpPool->AllocString("");

		return nullptr;
	}
	catch(Exception^ e)
	{
		return CreateExceptionSvnError("Commit log", e);
	}
	finally
	{
		ea->Detach(false);

		delete tmpPool;
	}
}


void SvnClientCallBacks::svn_wc_notify_func2(void *baton, const svn_wc_notify_t *notify, apr_pool_t *pool)
{
	UNUSED_ALWAYS(pool);
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnClientNotifyEventArgs^ ea = gcnew SvnClientNotifyEventArgs(notify);

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
	UNUSED_ALWAYS(pool);
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnClientProgressEventArgs^ ea = gcnew SvnClientProgressEventArgs(progress, total);

	try
	{
		client->HandleClientProgress(ea);
	}
	finally
	{
		ea->Detach(false);
	}
}

svn_error_t* SvnClientCallBacks::svn_wc_conflict_resolver_func(svn_wc_conflict_result_t *result, const svn_wc_conflict_description_t *description, void *baton, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	AprPool tmpPool(pool, false);

	SvnClientConflictResolveEventArgs^ ea = gcnew SvnClientConflictResolveEventArgs(description, %tmpPool);

	try
	{
		client->HandleClientConflictResolver(ea);

		if(ea->Cancel)
			return svn_error_create(SVN_ERR_CANCELLED, NULL, "Operation canceled");

		if(ea->Result != SvnConflictResult::Conflicted)
			*result = (svn_wc_conflict_result_t)ea->Result;

		return nullptr;
	}
	catch(Exception^ e)
	{
		return CreateExceptionSvnError("Conflict resolver", e);
	}
	finally
	{
		ea->Detach(false);
	}
}

Uri^ SvnClient::GetUriFromWorkingCopy(String^ path)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	EnsureState(SvnContextState::ConfigLoaded);

	AprPool pool(_pool);

	const char* url = nullptr;

	svn_error_t* err = svn_client_url_from_path(&url, pool.AllocString(path), pool.Handle);

	if(!err && url)
	{
		String ^uriStr = Utf8_PtrToString(url);

		return gcnew Uri(uriStr);
	}
	else
		return nullptr;
}

bool SvnClient::GetUuidFromUri(Uri^ uri, [Out] Guid% uuid)
{
	if(!uri)
		throw gcnew ArgumentNullException("uri");

	uuid = Guid::Empty;

	EnsureState(SvnContextState::AuthorizationInitialized);

	AprPool pool(_pool);

	const char* uuidStr = nullptr;

	svn_error_t* err = svn_client_uuid_from_url(&uuidStr, pool.AllocString(uri->ToString()), CtxHandle, pool.Handle);

	if(err || !uuidStr)
		return false;
	else
	{
		uuid = Guid(Utf8_PtrToString(uuidStr));
		return true;
	}
}