#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::Log(SvnTarget^ target, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!logHandler)
		throw gcnew ArgumentNullException("logHandler");

	InternalLog(NewSingleItemCollection(target->TargetName), target->Revision, gcnew SvnLogArgs(), logHandler);
}

bool SvnClient::Log(SvnTarget^ target, EventHandler<SvnLogEventArgs^>^ logHandler, SvnLogArgs^ args)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	array<String^>^ targets = gcnew array<String^>(1);

	targets[0] = target->TargetName;

	return InternalLog(NewSingleItemCollection(target->TargetName), target->Revision, args, logHandler);
}

void SvnClient::Log(SvnUriTarget^ baseTarget, ICollection<Uri^>^ relativeTargets, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	Log(baseTarget, relativeTargets, logHandler, gcnew SvnLogArgs());
}

bool SvnClient::Log(SvnUriTarget^ baseTarget, ICollection<Uri^>^ relativeTargets, EventHandler<SvnLogEventArgs^>^ logHandler, SvnLogArgs^ args)
{
	if(!baseTarget)
		throw gcnew ArgumentNullException("baseTarget");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	array<String^>^ targets;

	if(!relativeTargets || !relativeTargets->Count)
		targets = gcnew array<String^>(1);
	else
	{
		targets = gcnew array<String^>(1 + relativeTargets->Count);
		Uri^ baseUri = baseTarget->Uri;

		int i = 1;
		for each(Uri^ uri in relativeTargets)
		{
			if(!uri)
				throw gcnew ArgumentException("One of the relative uri's is null", "relativeTargets");

			Uri^ relUri = baseUri->MakeRelativeUri(gcnew Uri(baseUri, uri));

			if(relUri->IsAbsoluteUri)
				throw gcnew ArgumentException("One of the relative uri's is not relative", "relativeTargets");

			targets[i] = relUri->ToString();
			i++;
		}
	}

	targets[0] = baseTarget->TargetName;

	return InternalLog(safe_cast<ICollection<String^>^>(targets), baseTarget->Revision, args, logHandler);
}

void SvnClient::GetLog(SvnTarget^ target, [Out] IList<SvnLogEventArgs^>^% logItems)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	InfoItemList<SvnLogEventArgs^>^ results = gcnew InfoItemList<SvnLogEventArgs^>();

	try
	{
		Log(target, results->Handler);	
	}
	finally
	{
		logItems = safe_cast<IList<SvnLogEventArgs^>^>(results);
	}
}

bool SvnClient::GetLog(SvnTarget^ target, SvnLogArgs^ args, [Out] IList<SvnLogEventArgs^>^% logItems)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	InfoItemList<SvnLogEventArgs^>^ results = gcnew InfoItemList<SvnLogEventArgs^>();

	try
	{
		return Log(target, results->Handler, args);
	}
	finally
	{
		logItems = safe_cast<IList<SvnLogEventArgs^>^>(results);
	}
}

void SvnClient::GetLog(SvnUriTarget^ baseTarget, ICollection<Uri^>^ subTargets, [Out] IList<SvnLogEventArgs^>^% logItems)
{
	if(!baseTarget)
		throw gcnew ArgumentNullException("baseTarget");

	InfoItemList<SvnLogEventArgs^>^ results = gcnew InfoItemList<SvnLogEventArgs^>();

	try
	{
		return Log(baseTarget, subTargets, results->Handler);
	}
	finally
	{
		logItems = safe_cast<IList<SvnLogEventArgs^>^>(results);
	}
}

bool SvnClient::GetLog(SvnUriTarget^ baseTarget, ICollection<Uri^>^ subTargets, SvnLogArgs^ args, [Out] IList<SvnLogEventArgs^>^% logItems)
{
	if(!baseTarget)
		throw gcnew ArgumentNullException("baseTarget");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	InfoItemList<SvnLogEventArgs^>^ results = gcnew InfoItemList<SvnLogEventArgs^>();

	try
	{
		return Log(baseTarget, subTargets, results->Handler, args);
	}
	finally
	{
		logItems = safe_cast<IList<SvnLogEventArgs^>^>(results);
	}
}



static svn_error_t *svnclient_log_handler(void *baton, svn_log_entry_t *log_entry, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnLogArgs^ args = dynamic_cast<SvnLogArgs^>(client->CurrentArgs); // C#: _currentArgs as SvnLogArgs
	AprPool^ aprPool = AprPool::Attach(pool, false);
	if(args)
	{
		SvnLogEventArgs^ e = gcnew SvnLogEventArgs(log_entry, aprPool);

		/* date: use svn_time_from_cstring() if need apr_time_t */
		try
		{
			args->OnLog(e);

			if(e->Cancel)
				return svn_error_create(SVN_ERR_CEASE_INVOCATION, NULL, "Log receiver canceled operation");
		}
		catch(Exception^ e)
		{
			return svn_error_create(SVN_ERR_CANCELLED, NULL, aprPool->AllocString(String::Concat("Log receiver throwed exception: ", e->ToString())));
		}
		finally
		{
			e->Detach(false);
			delete aprPool; // Just detach
		}
	}
	return NULL;
}

bool SvnClient::InternalLog(ICollection<String^>^ targetStrings, SvnRevision^ pegRevision, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if(!targetStrings)
		throw gcnew ArgumentNullException("targetStrings");
	else if(!pegRevision)
		throw gcnew ArgumentNullException("pegRevision");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(_pool);
	_currentArgs = args;

	if(logHandler)
		args->Log += logHandler;
	try
	{
		AprArray<String^, AprCStrMarshaller^>^ targets = gcnew AprArray<String^, AprCStrMarshaller^>(targetStrings, %pool);

		svn_opt_revision_t pegRev = pegRevision->ToSvnRevision();
		svn_opt_revision_t start = args->Start->ToSvnRevision();
		svn_opt_revision_t end = args->End->ToSvnRevision();

		svn_error_t *r = svn_client_log4(
			targets->Handle,
			&pegRev,
			&start,
			&end,
			args->Limit,
			args->LogChangedPaths,
			args->StrictHistory,
			args->IncludeMergedRevisions,
			args->OmitMessages,
			svnclient_log_handler,
			(void*)_clientBatton->Handle,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;

		if(logHandler)
			args->Log -= logHandler;
	}
}