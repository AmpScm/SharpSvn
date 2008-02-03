// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#include "SvnAll.h"

using namespace SharpSvn;
using namespace SharpSvn::Apr;

using System::Diagnostics::CodeAnalysis::SuppressMessageAttribute;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetUuidFromUri(System.Uri,System.Guid&):System.Boolean", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1047:DoNotDeclareProtectedMembersInSealedTypes", Scope="member", Target="SharpSvn.SvnClient+ArgsStore.Dispose(System.Boolean):System.Void")];

SvnClient::SvnClient()
: _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
	_clientBatton = gcnew AprBaton<SvnClient^>(this);
	Initialize();
}

SvnClient::~SvnClient()
{
	delete _clientBatton;
}

struct SvnClientCallBacks
{
	static svn_error_t * __cdecl svn_cancel_func(void *cancel_baton);
	static svn_error_t *__cdecl svn_client_get_commit_log3(const char **log_msg, const char **tmp_file, const apr_array_header_t *commit_items, void *baton, apr_pool_t *pool);
	static void __cdecl svn_wc_notify_func2(void *baton, const svn_wc_notify_t *notify, apr_pool_t *pool);
	static void __cdecl svn_ra_progress_notify_func(apr_off_t progress, apr_off_t total, void *baton, apr_pool_t *pool);

	static svn_error_t * __cdecl svn_wc_conflict_resolver_func(svn_wc_conflict_result_t **result, const svn_wc_conflict_description_t *description, void *baton, apr_pool_t *pool);
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

System::Version^ SvnClient::SharpSvnVersion::get()
{
	return (gcnew System::Reflection::AssemblyName(SvnClient::typeid->Assembly->FullName))->Version;
}

void SvnClient::HandleClientCancel(SvnCancelEventArgs^ e)
{
	if(_currentArgs)
		_currentArgs->OnCancel(e);

	if(e->Cancel)
		return;

	OnCancel(e);
}

void SvnClient::OnCancel(SvnCancelEventArgs^ e)
{
	Cancel(this, e);
}

void SvnClient::HandleClientProgress(SvnProgressEventArgs^ e)
{
	if(_currentArgs)
		_currentArgs->OnProgress(e);

	OnProgress(e);
}

void SvnClient::OnProgress(SvnProgressEventArgs^ e)
{
	Progress(this, e);
}

void SvnClient::HandleClientGetCommitLog(SvnCommittingEventArgs^ e)
{
	if(e->Cancel)
		return;

	SvnClientArgsWithCommit^ commitArgs = dynamic_cast<SvnClientArgsWithCommit^>(CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs

	if(commitArgs)
		commitArgs->OnCommitting(e);

	if(e->Cancel)
		return;

	OnCommitting(e);
}

void SvnClient::OnCommitting(SvnCommittingEventArgs^ e)
{
	Committing(this, e);
}

void SvnClient::HandleClientNotify(SvnNotifyEventArgs^ e)
{
	if(_currentArgs)
		_currentArgs->OnNotify(e);

	OnNotify(e);
}

void SvnClient::OnNotify(SvnNotifyEventArgs^ e)
{
	Notify(this, e);
}

void SvnClient::HandleClientConflictResolver(SvnConflictEventArgs^ e)
{
	if(e->Cancel)
		return;

	SvnClientArgsWithConflict^ conflictArgs = dynamic_cast<SvnClientArgsWithConflict^>(CurrentCommandArgs); // C#: _currentArgs as SvnClientArgsWithConflict

	if(conflictArgs)
	{
		conflictArgs->OnConflict(e);

		if(e->Cancel)
			return;
	}
	
	OnConflict(e);
}

void SvnClient::OnConflict(SvnConflictEventArgs^ e)
{
	Conflict(this, e);
}

void SvnClient::HandleClientError(SvnErrorEventArgs^ e)
{
	if(e->Cancel)
		return;

	if(_currentArgs)
	{
		_currentArgs->OnSvnError(e);

		if(e->Cancel)
			return;
	}

	OnSvnError(e);
}

void SvnClient::OnSvnError(SvnErrorEventArgs^ e)
{
	SvnError(this, e);
}

void SvnClient::HandleBeforeCommand(SvnBeforeCommandEventArgs^ e)
{
	OnBeforeCommand(e);
}

void SvnClient::OnBeforeCommand(SvnBeforeCommandEventArgs^ e)
{
	BeforeCommand(this, e);
}

svn_error_t* SvnClientCallBacks::svn_cancel_func(void *cancel_baton)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)cancel_baton);

	SvnCancelEventArgs^ ea = gcnew SvnCancelEventArgs();
	try
	{
		client->HandleClientCancel(ea);

		if(ea->Cancel)
			return svn_error_create (SVN_ERR_CANCELLED, nullptr, "Operation canceled");

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

svn_error_t* SvnClientCallBacks::svn_client_get_commit_log3(const char **log_msg, const char **tmp_file, const apr_array_header_t *commit_items, void *baton, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	AprPool^ tmpPool = gcnew AprPool(pool, false);

	SvnCommittingEventArgs^ ea = gcnew SvnCommittingEventArgs(commit_items, tmpPool);

	*log_msg = nullptr;
	*tmp_file = nullptr;

	try
	{
		client->HandleClientGetCommitLog(ea);

		if(ea->Cancel)
			return svn_error_create (SVN_ERR_CANCELLED, nullptr, "Operation canceled");
		else if(ea->LogMessage)
			*log_msg = tmpPool->AllocUnixString(ea->LogMessage);
		else if(client->Configuration->LogMessageRequired)
			return svn_error_create (SVN_ERR_CANCELLED, nullptr, "Commit canceled: A logmessage is required");
		else
			*log_msg = tmpPool->AllocString("");

		return nullptr;
	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("Commit log", e);
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

	SvnNotifyEventArgs^ ea = gcnew SvnNotifyEventArgs(notify);

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

	SvnProgressEventArgs^ ea = gcnew SvnProgressEventArgs(progress, total);

	try
	{
		client->HandleClientProgress(ea);
	}
	finally
	{
		ea->Detach(false);
	}
}

svn_error_t* SvnClientCallBacks::svn_wc_conflict_resolver_func(svn_wc_conflict_result_t **result, const svn_wc_conflict_description_t *description, void *baton, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	*result = svn_wc_create_conflict_result(svn_wc_conflict_choose_postpone, NULL, pool);

	AprPool tmpPool(pool, false);

	SvnConflictEventArgs^ ea = gcnew SvnConflictEventArgs(description);

	try
	{
		client->HandleClientConflictResolver(ea);

		if(ea->Cancel)
			return svn_error_create(SVN_ERR_CANCELLED, nullptr, "Operation canceled");

		if(ea->Choice != SvnConflictChoice::Postpone)
		{
			(*result)->choice = (svn_wc_conflict_choice_t)ea->Choice;

			if(ea->Choice == SvnConflictChoice::Merged)
				(*result)->merged_file = tmpPool.AllocPath(ea->MergedFile);
		}

		return nullptr;
	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("Conflict resolver", e);
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

	AprPool pool(%_pool);

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
	else if(!SvnBase::IsValidReposUri(uri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

	uuid = Guid::Empty;

	EnsureState(SvnContextState::AuthorizationInitialized);

	AprPool pool(%_pool);

	const char* uuidStr = nullptr;

	svn_error_t* err = svn_client_uuid_from_url(&uuidStr, pool.AllocCanonical(uri->ToString()), CtxHandle, pool.Handle);

	if(err || !uuidStr)
		return false;
	else
	{
		uuid = Guid(Utf8_PtrToString(uuidStr));
		return true;
	}
}

Uri^ SvnClient::GetRepositoryRoot(Uri^ uri)
{
	if(!uri)
		throw gcnew ArgumentNullException("uri");
	else if(!SvnBase::IsValidReposUri(uri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

	const char* resultUrl = nullptr;
	EnsureState(SvnContextState::AuthorizationInitialized);
	AprPool pool(%_pool);

	svn_error_t* err = svn_client_root_url_from_path(&resultUrl, pool.AllocCanonical(uri->ToString()), CtxHandle, pool.Handle);

	if(!err && resultUrl)
	{
		String^ urlString = Utf8_PtrToString(resultUrl);

		if(!urlString->EndsWith("/", StringComparison::InvariantCulture))
			urlString += "/";

		if(Uri::TryCreate(urlString, UriKind::Absolute, uri))
		{
			return uri;
		}
	}
	return nullptr;
}

[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.GetRepositoryRoot(System.String):System.Uri")];

Uri^ SvnClient::GetRepositoryRoot(String^ path)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	const char* resultUrl = nullptr;
	EnsureState(SvnContextState::AuthorizationInitialized);
	AprPool pool(%_pool);

	svn_error_t* err = svn_client_root_url_from_path(&resultUrl, pool.AllocPath(path), CtxHandle, pool.Handle);

	if(!err && resultUrl)
	{
		Uri^ uri = nullptr;
		String^ urlString = Utf8_PtrToString(resultUrl);

		if(!urlString->EndsWith("/", StringComparison::InvariantCulture))
			urlString += "/";

		if(Uri::TryCreate(urlString, UriKind::Absolute, uri))
		{
			return uri;
		}
	}
	return nullptr;
}


bool SvnClientConfiguration::LogMessageRequired::get()
{
	return !_client->_noLogMessageRequired;
}
void SvnClientConfiguration::LogMessageRequired::set(bool value)
{
	_client->_noLogMessageRequired = !value;
}

SvnClient::ArgsStore::ArgsStore(SvnClient^ client, SvnClientArgs^ args)
{
	if(!args)
		throw gcnew ArgumentNullException("args");
	else if(client->_currentArgs)
		throw gcnew InvalidOperationException(SharpSvnStrings::SvnClientOperationInProgress);

	args->Exception = nullptr;
	client->_currentArgs = args;
	_client = client;
	try
	{
		client->HandleBeforeCommand(gcnew SvnBeforeCommandEventArgs(args));
	}
	catch(Exception^)
	{
		client->_currentArgs = nullptr;
		throw;
	}
}

void SvnClient::AddClientName(String^ name, System::Version^ version)
{
	if(String::IsNullOrEmpty(name))
		throw gcnew ArgumentNullException("name");
	else if(!version)
		throw gcnew ArgumentNullException("version");

	for(int i = 0; i < name->Length; i++)
	{
		if(!wchar_t::IsLetterOrDigit(name, i) && 0 > (((String^)"._ ")->IndexOf(name[i])))
			throw gcnew ArgumentException(SharpSvnStrings::InvalidCharacterInClientName, "name");
	}

	if(!_clientNames->Contains(name))
	{
		_clientNames->Add(name);

		_clientName += " " + name + "/" + version->ToString();
	}
}