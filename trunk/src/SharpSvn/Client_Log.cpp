// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;


[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(SharpSvn.SvnTarget,SharpSvn.SvnLogArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(SharpSvn.SvnTarget,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(SharpSvn.SvnUriTarget,System.Collections.Generic.ICollection`1<System.Uri>,SharpSvn.SvnLogArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetLog(SharpSvn.SvnUriTarget,System.Collections.Generic.ICollection`1<System.Uri>,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnLogEventArgs>&)", MessageId="2#")];
bool SvnClient::Log(SvnTarget^ target, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!logHandler)
		throw gcnew ArgumentNullException("logHandler");

	return InternalLog(NewSingleItemCollection(target->TargetName), target->Revision, gcnew SvnLogArgs(), logHandler);
}

bool SvnClient::Log(SvnTarget^ target, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return InternalLog(NewSingleItemCollection(target->TargetName), target->Revision, args, logHandler);
}

bool SvnClient::Log(SvnUriTarget^ baseTarget, ICollection<Uri^>^ relativeTargets, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	return Log(baseTarget, relativeTargets, gcnew SvnLogArgs(), logHandler);
}

bool SvnClient::Log(SvnUriTarget^ baseTarget, ICollection<Uri^>^ relativeTargets, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
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
				throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "relativeTargets");

			Uri^ relUri = baseUri->MakeRelativeUri(gcnew Uri(baseUri, uri));

			if(relUri->IsAbsoluteUri)
				throw gcnew ArgumentException(SharpSvnStrings::InvalidUri, "relativeTargets");

			targets[i] = relUri->ToString();
			i++;
		}
	}

	targets[0] = baseTarget->TargetName;

	return InternalLog(safe_cast<ICollection<String^>^>(targets), baseTarget->Revision, args, logHandler);
}

bool SvnClient::GetLog(SvnTarget^ target, [Out] Collection<SvnLogEventArgs^>^% logItems)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	InfoItemCollection<SvnLogEventArgs^>^ results = gcnew InfoItemCollection<SvnLogEventArgs^>();

	try
	{
		return Log(target, results->Handler);
	}
	finally
	{
		logItems = results;
	}
}

bool SvnClient::GetLog(SvnTarget^ target, SvnLogArgs^ args, [Out] Collection<SvnLogEventArgs^>^% logItems)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnLogEventArgs^>^ results = gcnew InfoItemCollection<SvnLogEventArgs^>();

	try
	{
		return Log(target, args, results->Handler);
	}
	finally
	{
		logItems = results;
	}
}

bool SvnClient::GetLog(SvnUriTarget^ baseTarget, ICollection<Uri^>^ subTargets, [Out] Collection<SvnLogEventArgs^>^% logItems)
{
	if(!baseTarget)
		throw gcnew ArgumentNullException("baseTarget");

	InfoItemCollection<SvnLogEventArgs^>^ results = gcnew InfoItemCollection<SvnLogEventArgs^>();

	try
	{
		return Log(baseTarget, subTargets, results->Handler);
	}
	finally
	{
		logItems = results;
	}
}

bool SvnClient::GetLog(SvnUriTarget^ baseTarget, ICollection<Uri^>^ subTargets, SvnLogArgs^ args, [Out] Collection<SvnLogEventArgs^>^% logItems)
{
	if(!baseTarget)
		throw gcnew ArgumentNullException("baseTarget");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnLogEventArgs^>^ results = gcnew InfoItemCollection<SvnLogEventArgs^>();

	try
	{
		return Log(baseTarget, subTargets, args, results->Handler);
	}
	finally
	{
		logItems = results;
	}
}

static svn_error_t *svnclient_log_handler(void *baton, svn_log_entry_t *log_entry, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnLogArgs^ args = dynamic_cast<SvnLogArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnLogArgs
	AprPool aprPool(pool, false);
	if(args)
	{
		if(log_entry->revision == SVN_INVALID_REVNUM)
		{
			// This marks the end of logs at this level,
			args->_mergeLogLevel--;
			return nullptr;
		}

		SvnLogEventArgs^ e = gcnew SvnLogEventArgs(log_entry, args->_mergeLogLevel, %aprPool);

		if(log_entry->has_children)
			args->_mergeLogLevel++;

		try
		{
			args->OnLog(e);

			if(e->Cancel)
				return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Log receiver canceled operation");
		}
		catch(Exception^ e)
		{
			return SvnException::CreateExceptionSvnError("Log receiver", e);
		}
		finally
		{
			e->Detach(false);
		}
	}
	return nullptr;
}

bool SvnClient::InternalLog(ICollection<String^>^ targets, SvnRevision^ pegRevision, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if(!targets)
		throw gcnew ArgumentNullException("targets");
	else if(!pegRevision)
		throw gcnew ArgumentNullException("pegRevision");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	args->_mergeLogLevel = 0; // Clear log level
	if(logHandler)
		args->Log += logHandler;
	try
	{
		apr_array_header_t* retrieveProperties;

		if(args->RetrievePropertiesUsed)
			retrieveProperties = AllocArray(args->RetrieveProperties, %pool);
		else
			retrieveProperties = svn_compat_log_revprops_in(pool.Handle);

		svn_opt_revision_t pegRev = pegRevision->ToSvnRevision();
		svn_opt_revision_t start = args->Start->ToSvnRevision(SvnRevision::Head);
		svn_opt_revision_t end = args->End->ToSvnRevision(SvnRevision::Zero);

		svn_error_t *r = svn_client_log4(
			AllocArray(targets, %pool),
			&pegRev,
			&start,
			&end,
			args->Limit,
			args->LogChangedPaths,
			args->StrictNodeHistory,
			args->IncludeMergedRevisions,
			retrieveProperties,
			svnclient_log_handler,
			(void*)_clientBatton->Handle,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(this, r);
	}
	finally
	{
		if(logHandler)
			args->Log -= logHandler;
	}
}
