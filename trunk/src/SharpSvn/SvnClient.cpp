#include "stdafx.h"

#include "SvnAll.h"

using namespace SharpSvn;
using namespace SharpSvn::Apr;

using System::Diagnostics::CodeAnalysis::SuppressMessageAttribute;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetLog(SharpSvn.SvnUriTarget,System.Collections.Generic.ICollection`1<System.Uri>,SharpSvn.SvnLogArgs,System.Collections.Generic.IList`1<SharpSvn.SvnLogEventArgs>&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetMergeInfo(SharpSvn.SvnTarget,SharpSvn.SvnGetMergeInfoArgs,SharpSvn.SvnMergeInfo&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetLog(SharpSvn.SvnTarget,SharpSvn.SvnLogArgs,System.Collections.Generic.IList`1<SharpSvn.SvnLogEventArgs>&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Switch(System.String,SharpSvn.SvnUriTarget,SharpSvn.SvnSwitchArgs,System.Int64&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetUuidFromUri(System.Uri,System.Guid&):System.Boolean", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetChangeList(System.String,System.String,SharpSvn.SvnListChangeListArgs,System.Collections.Generic.IList`1<System.String>&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Update(System.String,System.Int64&):System.Void", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Diff(SharpSvn.SvnTarget,SharpSvn.SvnRevision,SharpSvn.SvnRevision,System.IO.FileStream&):System.Void", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Update(System.Collections.Generic.ICollection`1<System.String>,SharpSvn.SvnUpdateArgs,System.Collections.Generic.IList`1<System.Int64>&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetList(SharpSvn.SvnTarget,SharpSvn.SvnListArgs,System.Collections.Generic.IList`1<SharpSvn.SvnListEventArgs>&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetPropertyList(SharpSvn.SvnTarget,SharpSvn.SvnPropertyListArgs,System.Collections.Generic.IList`1<SharpSvn.SvnPropertyListEventArgs>&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteDelete(System.Uri,SharpSvn.SvnDeleteArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetStatus(System.String,SharpSvn.SvnStatusEventArgs&):System.Void", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Commit(System.Collections.Generic.ICollection`1<System.String>,SharpSvn.SvnCommitArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteCopy(SharpSvn.SvnUriTarget,System.Uri,SharpSvn.SvnCommitInfo&):System.Void", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteMove(System.Uri,System.Uri,SharpSvn.SvnCommitInfo&):System.Void", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetInfo(SharpSvn.SvnTarget,SharpSvn.SvnInfoArgs,System.Collections.Generic.IList`1<SharpSvn.SvnInfoEventArgs>&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteMove(System.Uri,System.Uri,SharpSvn.SvnMoveArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.CheckOut(SharpSvn.SvnUriTarget,System.String,System.Int64&):System.Void", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Import(System.String,System.Uri,SharpSvn.SvnCommitInfo&):System.Void", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Diff(SharpSvn.SvnTarget,SharpSvn.SvnTarget,System.IO.FileStream&):System.Void", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Commit(System.String,SharpSvn.SvnCommitArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Update(System.String,SharpSvn.SvnUpdateArgs,System.Int64&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteCreateDirectory(System.Uri,SharpSvn.SvnCreateDirectoryArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetList(SharpSvn.SvnTarget,System.Collections.Generic.IList`1<SharpSvn.SvnListEventArgs>&):System.Void", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Diff(SharpSvn.SvnTarget,SharpSvn.SvnTarget,SharpSvn.SvnDiffArgs,System.IO.FileStream&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteCopy(SharpSvn.SvnUriTarget,System.Uri,SharpSvn.SvnCopyArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetProperty(SharpSvn.SvnTarget,System.String,SharpSvn.SvnGetPropertyArgs,System.Collections.Generic.IDictionary`2<SharpSvn.SvnTarget,System.Collections.Generic.IList`1<optional(System.Runtime.CompilerServices.IsSignUnspecifiedByte) System.SByte>>&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetChangeList(System.String,System.String,System.Collections.Generic.IList`1<SharpSvn.SvnListChangeListEventArgs>&):System.Void", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteMove(System.Collections.Generic.ICollection`1<System.Uri>,System.Uri,SharpSvn.SvnMoveArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteImport(System.String,System.Uri,SharpSvn.SvnCommitInfo&):System.Void", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteCopy(System.Collections.Generic.ICollection`1<SharpSvn.SvnUriTarget>,System.Uri,SharpSvn.SvnCopyArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetPropertyList(SharpSvn.SvnTarget,System.Collections.Generic.IList`1<SharpSvn.SvnPropertyListEventArgs>&):System.Void", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteDelete(System.Collections.Generic.ICollection`1<System.Uri>,SharpSvn.SvnDeleteArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Diff(SharpSvn.SvnTarget,SharpSvn.SvnRevision,SharpSvn.SvnRevision,SharpSvn.SvnDiffArgs,System.IO.FileStream&):System.Boolean", MessageId="4#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Import(System.String,System.Uri,SharpSvn.SvnImportArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetLog(SharpSvn.SvnTarget,System.Collections.Generic.IList`1<SharpSvn.SvnLogEventArgs>&):System.Void", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetChangeList(System.String,System.String,System.Collections.Generic.IList`1<System.String>&):System.Void", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetLog(SharpSvn.SvnUriTarget,System.Collections.Generic.ICollection`1<System.Uri>,System.Collections.Generic.IList`1<SharpSvn.SvnLogEventArgs>&):System.Void", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Update(System.Collections.Generic.ICollection`1<System.String>,System.Collections.Generic.IList`1<System.Int64>&):System.Void", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteMove(System.Collections.Generic.ICollection`1<System.Uri>,System.Uri,SharpSvn.SvnCommitInfo&):System.Void", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetDiffSummary(SharpSvn.SvnTarget,SharpSvn.SvnTarget,System.Collections.Generic.IList`1<SharpSvn.SvnDiffSummaryEventArgs>&):System.Void", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Export(SharpSvn.SvnTarget,System.String,System.Int64&):System.Void", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Commit(System.String,SharpSvn.SvnCommitInfo&):System.Void", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteCopy(System.Collections.Generic.ICollection`1<SharpSvn.SvnUriTarget>,System.Uri,SharpSvn.SvnCommitInfo&):System.Void", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.CheckOut(SharpSvn.SvnUriTarget,System.String,SharpSvn.SvnCheckOutArgs,System.Int64&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetChangeList(System.String,System.String,SharpSvn.SvnListChangeListArgs,System.Collections.Generic.IList`1<SharpSvn.SvnListChangeListEventArgs>&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetInfo(SharpSvn.SvnTarget,SharpSvn.SvnInfoEventArgs&):System.Void", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Commit(System.Collections.Generic.ICollection`1<System.String>,SharpSvn.SvnCommitInfo&):System.Void", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetDiffSummary(SharpSvn.SvnTarget,SharpSvn.SvnTarget,SharpSvn.SvnDiffSummaryArgs,System.Collections.Generic.IList`1<SharpSvn.SvnDiffSummaryEventArgs>&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteImport(System.String,System.Uri,SharpSvn.SvnImportArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetStatus(System.String,SharpSvn.SvnStatusArgs,System.Collections.Generic.IList`1<SharpSvn.SvnStatusEventArgs>&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteCreateDirectory(System.Collections.Generic.ICollection`1<System.Uri>,SharpSvn.SvnCreateDirectoryArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Switch(System.String,SharpSvn.SvnUriTarget,System.Int64&):System.Void", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetStatus(System.String,System.Collections.Generic.IList`1<SharpSvn.SvnStatusEventArgs>&):System.Void", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetProperty(SharpSvn.SvnTarget,System.String,System.Collections.Generic.IList`1<optional(System.Runtime.CompilerServices.IsSignUnspecifiedByte) System.SByte>&):System.Void", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Export(SharpSvn.SvnTarget,System.String,SharpSvn.SvnExportArgs,System.Int64&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetMergeInfo(SharpSvn.SvnTarget,SharpSvn.SvnMergeInfo&):System.Void", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetProperty(SharpSvn.SvnTarget,System.String,SharpSvn.SvnGetPropertyArgs,System.Collections.Generic.IDictionary`2<SharpSvn.SvnTarget,System.String>&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetProperty(SharpSvn.SvnTarget,System.String,System.String&):System.Void", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Export(SharpSvn.SvnTarget,System.String,System.Boolean,System.Int64&):System.Void", MessageId="3#")];

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
	SvnClientArgsWithCommit^ commitArgs = dynamic_cast<SvnClientArgsWithCommit^>(CurrentArgs); // C#: _currentArgs as SvnCommitArgs

	if(commitArgs)
		commitArgs->OnCommitting(e);

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
	OnConflict(e);
}

void SvnClient::OnConflict(SvnConflictEventArgs^ e)
{
	Conflict(this, e);
}

svn_error_t* SvnClientCallBacks::svn_cancel_func(void *cancel_baton)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)cancel_baton);

	SvnCancelEventArgs^ ea = gcnew SvnCancelEventArgs();
	try
	{
		client->HandleClientCancel(ea);

		if(ea->Cancel)
			return svn_error_create (SVN_ERR_CANCELLED, NULL, "Operation canceled");

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
			return svn_error_create (SVN_ERR_CANCELLED, NULL, "Operation canceled");
		else if(ea->LogMessage)
			*log_msg = tmpPool->AllocUnixString(ea->LogMessage);
		else if(client->Configuration->LogMessageRequired)
			return svn_error_create (SVN_ERR_CANCELLED, NULL, "Commit canceled: A logmessage is required");
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

svn_error_t* SvnClientCallBacks::svn_wc_conflict_resolver_func(svn_wc_conflict_result_t *result, const svn_wc_conflict_description_t *description, void *baton, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	AprPool tmpPool(pool, false);

	SvnConflictEventArgs^ ea = gcnew SvnConflictEventArgs(description);

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

	uuid = Guid::Empty;

	EnsureState(SvnContextState::AuthorizationInitialized);

	AprPool pool(%_pool);

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

bool SvnClientConfiguration::LogMessageRequired::get()
{
	return !_client->_noLogMessageRequired;
}
void SvnClientConfiguration::LogMessageRequired::set(bool value)
{
	_client->_noLogMessageRequired = !value;
}