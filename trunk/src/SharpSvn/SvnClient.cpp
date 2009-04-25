// $Id$
//
// Copyright 2007-2009 The SharpSvn Project
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
#include "SvnLibraryAttribute.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace System::Collections::Generic;
using System::Diagnostics::CodeAnalysis::SuppressMessageAttribute;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetUuidFromUri(System.Uri,System.Guid&):System.Boolean", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetRepositoryIdFromUri(System.Uri,System.Guid&)", MessageId="1#")];

[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.#TryGetRepositoryId(System.String,System.Guid&)")];
[module: SuppressMessage("Microsoft.Performance", "CA1811:AvoidUncalledPrivateCode", Scope="member", Target="SharpSvn.SvnClient.#set_AdministrativeDirectoryName(System.String)")];

SvnClient::SvnClient()
: _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
	_clientBaton = gcnew AprBaton<SvnClient^>(this);
	Initialize();
}

SvnClient::~SvnClient()
{
	delete _clientBaton;
}

// Allow changing the AdministrativeDirectory for users willing to take the risks involved.
void SvnClient::AdministrativeDirectoryName::set(String^ value)
{
	if (String::IsNullOrEmpty(value))
		throw gcnew ArgumentNullException("value");

	AprPool pool(SmallThreadPool);

	SVN_THROW(svn_wc_set_adm_dir(pool.AllocString(value), pool.Handle));

	_admDir = svn_wc_get_adm_dir(pool.Handle);
	_administrativeDirName = Utf8_PtrToString(_admDir);
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
	void* baton = (void*)_clientBaton->Handle;

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
	if (CurrentCommandArgs)
		CurrentCommandArgs->RaiseOnCancel(e);

	if (e->Cancel)
		return;

	OnCancel(e);
}

void SvnClient::OnCancel(SvnCancelEventArgs^ e)
{
	Cancel(this, e);
}

void SvnClient::HandleClientProgress(SvnProgressEventArgs^ e)
{
	if (CurrentCommandArgs)
		CurrentCommandArgs->RaiseOnProgress(e);

	OnProgress(e);
}

void SvnClient::OnProgress(SvnProgressEventArgs^ e)
{
	Progress(this, e);
}

void SvnClient::HandleClientCommitting(SvnCommittingEventArgs^ e)
{
	if (e->Cancel)
		return;

	SvnClientArgsWithCommit^ commitArgs = dynamic_cast<SvnClientArgsWithCommit^>(CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs

	if (commitArgs)
		commitArgs->RaiseCommitting(e);

	if (e->Cancel)
		return;

	OnCommitting(e);
}

void SvnClient::OnCommitting(SvnCommittingEventArgs^ e)
{
	Committing(this, e);
}

void SvnClient::HandleClientNotify(SvnNotifyEventArgs^ e)
{
	if (CurrentCommandArgs)
		CurrentCommandArgs->RaiseOnNotify(e);

	OnNotify(e);
}

void SvnClient::OnNotify(SvnNotifyEventArgs^ e)
{
	Notify(this, e);
}

void SvnClient::HandleClientConflict(SvnConflictEventArgs^ e)
{
	if (e->Cancel)
		return;

	SvnClientArgsWithConflict^ conflictArgs = dynamic_cast<SvnClientArgsWithConflict^>(CurrentCommandArgs); // C#: _currentArgs as SvnClientArgsWithConflict

	if (conflictArgs)
	{
		conflictArgs->RaiseConflict(e);

		if (e->Cancel)
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

void SvnClient::OnSvnError(SvnErrorEventArgs^ e)
{
	SvnError(this, e);
}

void SvnClient::HandleProcessing(SvnProcessingEventArgs^ e)
{
	OnProcessing(e);
}

void SvnClient::OnProcessing(SvnProcessingEventArgs^ e)
{
	Processing(this, e);
}

svn_error_t* SvnClientCallBacks::svn_cancel_func(void *cancel_baton)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)cancel_baton);

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

svn_error_t* SvnClientCallBacks::svn_client_get_commit_log3(const char **log_msg, const char **tmp_file, const apr_array_header_t *commit_items, void *baton, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	AprPool^ tmpPool = gcnew AprPool(pool, false);

	SvnCommittingEventArgs^ ea = gcnew SvnCommittingEventArgs(commit_items, client->CurrentCommandArgs->CommandType, tmpPool);

	*log_msg = nullptr;
	*tmp_file = nullptr;

	try
	{
		client->HandleClientCommitting(ea);

		if (ea->Cancel)
			return svn_error_create (SVN_ERR_CANCELLED, nullptr, "Operation canceled from OnCommitting");
		else if (ea->LogMessage)
			*log_msg = tmpPool->AllocUnixString(ea->LogMessage);
		else if (!client->_noLogMessageRequired)
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

	AprPool tmpPool(pool, false); // Connect to parent pool

	SvnConflictEventArgs^ ea = gcnew SvnConflictEventArgs(description, %tmpPool);

	try
	{
		client->HandleClientConflict(ea);

		if (ea->Cancel)
			return svn_error_create(SVN_ERR_CANCELLED, nullptr, "Operation canceled from OnConflict");

		if (ea->Choice != SvnAccept::Postpone && ea->MergedFile)
		{
			(*result)->choice = (svn_wc_conflict_choice_t)ea->Choice;

			if (ea->Choice == SvnAccept::Merged)
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
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	path = SvnTools::GetNormalizedFullPath(path);

	AprPool pool(%_pool);

	const char* url = nullptr;

	svn_error_t* err = svn_client_url_from_path(&url, pool.AllocPath(path), pool.Handle);

	if (!err && url)
		return Utf8_PtrToUri(url, System::IO::Directory::Exists(path) ? SvnNodeKind::Directory : SvnNodeKind::File);
	else if (err)
		svn_error_clear(err);

	return nullptr;
}

bool SvnClient::TryGetRepositoryId(Uri^ uri, [Out] Guid% id)
{
	if (!uri)
		throw gcnew ArgumentNullException("uri");
	else if (!SvnBase::IsValidReposUri(uri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

	id = Guid::Empty;

	EnsureState(SvnContextState::AuthorizationInitialized);

	AprPool pool(%_pool);

	const char* uuidStr = nullptr;

	svn_error_t* err = svn_client_uuid_from_url(&uuidStr, pool.AllocCanonical(uri), CtxHandle, pool.Handle);

	if (err || !uuidStr)
	{
		if (err)
			svn_error_clear(err);
		return false;
	}
	else
	{
		id = Guid(Utf8_PtrToString(uuidStr));
		return true;
	}
}

bool SvnClient::TryGetRepositoryId(String^ path, [Out] Guid% id)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!SvnBase::IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	id = Guid::Empty;

	EnsureState(SvnContextState::AuthorizationInitialized);

	AprPool pool(%_pool);

	const char* pPath = pool.AllocPath(path);
	const char* uuidStr = nullptr;

	svn_wc_adm_access_t *adm = nullptr;
	svn_error_t* err = svn_wc_adm_probe_open3(	&adm, 
												nullptr, 
												pPath, 
												false,
												0,
												CtxHandle->cancel_func,
												CtxHandle->cancel_baton,
												pool.Handle);

	if (err || !adm)
	{
		svn_error_clear(err);
		return false;
	}

	err = svn_client_uuid_from_path(&uuidStr, pPath, adm, CtxHandle, pool.Handle);

	svn_error_clear(svn_wc_adm_close2(adm, pool.Handle));

	if (err || !uuidStr)
	{
		svn_error_clear(err);
		return false;
	}
	
	id = Guid(Utf8_PtrToString(uuidStr));
	return true;
}

Uri^ SvnClient::GetRepositoryRoot(Uri^ uri)
{
	if (!uri)
		throw gcnew ArgumentNullException("uri");
	else if (!SvnBase::IsValidReposUri(uri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

	const char* resultUrl = nullptr;
	EnsureState(SvnContextState::AuthorizationInitialized);
	AprPool pool(%_pool);

	svn_error_t* err = svn_client_root_url_from_path(&resultUrl, pool.AllocCanonical(uri), CtxHandle, pool.Handle);

	if (!err && resultUrl)
		return Utf8_PtrToUri(resultUrl, SvnNodeKind::Directory);
	else if (err)
		svn_error_clear(err);

	return nullptr;
}

[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.GetRepositoryRoot(System.String):System.Uri")];

Uri^ SvnClient::GetRepositoryRoot(String^ target)
{
	if (String::IsNullOrEmpty(target))
		throw gcnew ArgumentNullException("target");
	else if (!IsNotUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");

	const char* resultUrl = nullptr;
	EnsureState(SvnContextState::AuthorizationInitialized);
	AprPool pool(%_pool);

	svn_error_t* err = svn_client_root_url_from_path(&resultUrl, pool.AllocPath(target), CtxHandle, pool.Handle);

	if (!err && resultUrl)
		return Utf8_PtrToUri(resultUrl, SvnNodeKind::Directory);
	else if (err)
		svn_error_clear(err);

	return nullptr;
}

void SvnClient::AddClientName(String^ name, System::Version^ version)
{
	if (String::IsNullOrEmpty(name))
		throw gcnew ArgumentNullException("name");
	else if (!version)
		throw gcnew ArgumentNullException("version");

	for (int i = 0; i < name->Length; i++)
	{
		if (!wchar_t::IsLetterOrDigit(name, i) && 0 > (((String^)"._ ")->IndexOf(name[i])))
			throw gcnew ArgumentException(SharpSvnStrings::InvalidCharacterInClientName, "name");
	}

	if (!_clientNames->Contains(name))
	{
		_clientNames->Add(name);

		_clientName += " " + name + "/" + version->ToString();
	}
}

const char* SvnClient::GetEolPtr(SvnLineStyle style)
{
	switch(style)
	{
	case SvnLineStyle::Default:
	case SvnLineStyle::Native:
		return nullptr;
	case SvnLineStyle::CarriageReturnLinefeed:
		return "CRLF";
	case SvnLineStyle::Linefeed:
		return "LF";
	case SvnLineStyle::CarriageReturn:
		return "CR";
	default:
		throw gcnew ArgumentOutOfRangeException("style");
	}
}

const char* SvnClient::GetEolValue(SvnLineStyle style)
{
	switch(style)
	{
	case SvnLineStyle::Default:
		return nullptr;
	case SvnLineStyle::Native:
		return APR_EOL_STR;
	case SvnLineStyle::CarriageReturnLinefeed:
		return "\r\n";
	case SvnLineStyle::Linefeed:
		return "\n";
	case SvnLineStyle::CarriageReturn:
		return "\r";
	default:
		throw gcnew ArgumentOutOfRangeException("style");
	}
}

using SharpSvn::Implementation::SvnLibrary;
using SharpSvn::Implementation::SvnLibraryAttribute;

static int CompareLibrary(SvnLibrary^ x, SvnLibrary^ y)
{
	return StringComparer::OrdinalIgnoreCase->Compare(x->Name, y->Name);
}

ICollection<SvnLibrary^>^ SvnClient::SvnLibraries::get()
{
	if (_svnLibraries)
		return _svnLibraries;

	System::Collections::Generic::List<SvnLibrary^>^ libs 
		= gcnew System::Collections::Generic::List<SvnLibrary^>();

	for each(SvnLibraryAttribute^ i in SvnClient::typeid->Assembly->GetCustomAttributes(SvnLibraryAttribute::typeid, false))
	{
		libs->Add(gcnew SvnLibrary(i));
	}

	libs->Sort(gcnew Comparison<SvnLibrary^>(CompareLibrary));

	return _svnLibraries = libs->AsReadOnly();
}
