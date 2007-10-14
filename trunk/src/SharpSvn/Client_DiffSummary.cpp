// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;


bool SvnClient::DiffSummary(SvnTarget^ from, SvnTarget^ to, EventHandler<SvnDiffSummaryEventArgs^>^ summaryHandler)
{
	if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!to)
		throw gcnew ArgumentNullException("to");
	else if(!summaryHandler)
		throw gcnew ArgumentNullException("summaryHandler");

	return DiffSummary(from, to, gcnew SvnDiffSummaryArgs(), summaryHandler);
}

static svn_error_t *svn_client_diff_summarize_func_handler(const svn_client_diff_summarize_t *diff, void *baton, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	AprPool thePool(pool, false);

	SvnDiffSummaryArgs^ args = dynamic_cast<SvnDiffSummaryArgs^>(client->CurrentArgs); // C#: _currentArgs as SvnCommitArgs
	if(args)
	{
		SvnDiffSummaryEventArgs^ e = gcnew SvnDiffSummaryEventArgs(diff);
		try
		{
			args->OnSummaryHandler(e);

			if(e->Cancel)
				return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Diff summary receiver canceled operation");
		}
		catch(Exception^ e)
		{
			return SvnException:: CreateExceptionSvnError("Diff summary receiver", e);
		}
		finally
		{
			e->Detach(false);
		}
	}

	return nullptr;
}


bool SvnClient::DiffSummary(SvnTarget^ from, SvnTarget^ to, SvnDiffSummaryArgs^ args, EventHandler<SvnDiffSummaryEventArgs^>^ summaryHandler)
{
	if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!to)
		throw gcnew ArgumentNullException("to");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if(summaryHandler)
		args->SummaryHandler += summaryHandler;
	try
	{
		svn_opt_revision_t fromRev = from->GetSvnRevision(SvnRevision::Base, SvnRevision::Head);
		svn_opt_revision_t toRev = to->GetSvnRevision(SvnRevision::Base, SvnRevision::Head);

		svn_error_t *r = svn_client_diff_summarize2(
			pool.AllocString(from->TargetName),
			&fromRev,
			pool.AllocString(to->TargetName),
			&toRev,
			(svn_depth_t)args->Depth,
			args->IgnoreAncestry,
			svn_client_diff_summarize_func_handler,
			(void*)_clientBatton->Handle,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(this, r);
	}
	finally
	{
		if(summaryHandler)
			args->SummaryHandler -= summaryHandler;
	}
}

bool SvnClient::GetDiffSummary(SvnTarget^ from, SvnTarget^ to, [Out] IList<SvnDiffSummaryEventArgs^>^% list)
{
	if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!to)
		throw gcnew ArgumentNullException("to");

	InfoItemCollection<SvnDiffSummaryEventArgs^>^ results = gcnew InfoItemCollection<SvnDiffSummaryEventArgs^>();

	try
	{
		return DiffSummary(from, to, gcnew SvnDiffSummaryArgs(), results->Handler);
	}
	finally
	{
		list = safe_cast<IList<SvnDiffSummaryEventArgs^>^>(results);
	}
}

bool SvnClient::GetDiffSummary(SvnTarget^ from, SvnTarget^ to, SvnDiffSummaryArgs^ args, [Out] IList<SvnDiffSummaryEventArgs^>^% list)
{
	if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!to)
		throw gcnew ArgumentNullException("to");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnDiffSummaryEventArgs^>^ results = gcnew InfoItemCollection<SvnDiffSummaryEventArgs^>();

	try
	{
		return DiffSummary(from, to, args, results->Handler);
	}
	finally
	{
		list = safe_cast<IList<SvnDiffSummaryEventArgs^>^>(results);
	}
}
