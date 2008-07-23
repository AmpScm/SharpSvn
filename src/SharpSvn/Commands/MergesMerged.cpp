// $Id: GetEligableMergeInfo.cpp 517 2008-04-06 21:37:41Z rhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/MergesMerged.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetEligableMergeInfo(SharpSvn.SvnTarget,System.Uri,SharpSvn.SvnGetEligableMergeInfoArgs,SharpSvn.SvnEligableMergeInfo&)", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetEligableMergeInfo(SharpSvn.SvnTarget,System.Uri,SharpSvn.SvnEligableMergeInfo&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.#GetEligableMergeInfo(SharpSvn.SvnTarget,System.Uri,SharpSvn.SvnGetEligableMergeInfoArgs,SharpSvn.SvnEligableMergeInfo&)")];

bool SvnClient::ListMergesMerged(SvnTarget ^target, SvnUriTarget^ source, EventHandler<SvnMergesMergedEventArgs^>^ logHandler)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!source)
		throw gcnew ArgumentNullException("source");
	else if (!logHandler)
		throw gcnew ArgumentNullException("logHandler");
	
	return ListMergesMerged(target, source, gcnew SvnMergesMergedArgs(), logHandler);
}

static svn_error_t *svnclient_eligible_log_handler(void *baton, svn_log_entry_t *log_entry, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnMergesMergedArgs^ args = dynamic_cast<SvnMergesMergedArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnLogArgs
	AprPool aprPool(pool, false);
	if (!args)
		return nullptr;

	SvnMergesMergedEventArgs^ e = gcnew SvnMergesMergedEventArgs(log_entry, args->_sourceTarget, %aprPool);

	try
	{
		args->OnList(e);

		if (e->Cancel)
			return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Log receiver canceled operation");
		else
			return nullptr;
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


bool SvnClient::ListMergesMerged(SvnTarget ^target, SvnUriTarget^ source, SvnMergesMergedArgs^ args, EventHandler<SvnMergesMergedEventArgs^>^ handler)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!source)
		throw gcnew ArgumentNullException("source");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if (handler)
		args->List += handler;
	try
	{
		apr_array_header_t* retrieveProperties;

		if (args->RetrievePropertiesUsed)
			retrieveProperties = args->RetrieveProperties->Count ? AllocArray(args->RetrieveProperties, %pool) : nullptr;
		else
			retrieveProperties = svn_compat_log_revprops_in(pool.Handle);

		args->_sourceTarget = source;

		svn_error_t* r = svn_client_mergeinfo_log_merged(
			pool.AllocString(target->SvnTargetName),
			target->Revision->AllocSvnRevision(%pool),
			pool.AllocString(source->SvnTargetName),
			source->Revision->AllocSvnRevision(%pool),
			svnclient_eligible_log_handler,
			(void*)_clientBatton->Handle,
			args->RetrieveChangedPaths,
			retrieveProperties,
			CtxHandle,
			pool.Handle);

			return args->HandleResult(this, r);
	}
	finally
	{
		if (handler)
			args->List -= handler;

		args->_sourceTarget = nullptr;
	}
}

bool SvnClient::GetMergesMerged(SvnTarget ^target, SvnUriTarget^ source, [Out] Collection<SvnMergesMergedEventArgs^>^% list)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!source)
		throw gcnew ArgumentNullException("source");
	
	return GetMergesMerged(target, source, gcnew SvnMergesMergedArgs(), list);
}

bool SvnClient::GetMergesMerged(SvnTarget ^target, SvnUriTarget^ source, SvnMergesMergedArgs^ args, [Out] Collection<SvnMergesMergedEventArgs^>^% list)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!source)
		throw gcnew ArgumentNullException("source");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnMergesMergedEventArgs^>^ results = gcnew InfoItemCollection<SvnMergesMergedEventArgs^>();

	try
	{
		return ListMergesMerged(target, source, args, results->Handler);
	}
	finally
	{
		list = results;
	}
}