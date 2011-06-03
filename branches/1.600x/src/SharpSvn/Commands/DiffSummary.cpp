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
#include "Args/DiffSummary.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;


[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetDiffSummary(SharpSvn.SvnTarget,SharpSvn.SvnTarget,SharpSvn.SvnDiffSummaryArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnDiffSummaryEventArgs>&)", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetDiffSummary(SharpSvn.SvnTarget,SharpSvn.SvnTarget,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnDiffSummaryEventArgs>&)", MessageId="2#")];

bool SvnClient::DiffSummary(SvnTarget^ from, SvnTarget^ to, EventHandler<SvnDiffSummaryEventArgs^>^ summaryHandler)
{
	if (!from)
		throw gcnew ArgumentNullException("from");
	else if (!to)
		throw gcnew ArgumentNullException("to");
	else if (!summaryHandler)
		throw gcnew ArgumentNullException("summaryHandler");

	return DiffSummary(from, to, gcnew SvnDiffSummaryArgs(), summaryHandler);
}

static svn_error_t *svn_client_diff_summarize_func_handler(const svn_client_diff_summarize_t *diff, void *baton, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	AprPool thePool(pool, false);

	SvnDiffSummaryArgs^ args = dynamic_cast<SvnDiffSummaryArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs
	if (!args)
		return nullptr;

	SvnDiffSummaryEventArgs^ e = gcnew SvnDiffSummaryEventArgs(diff, args->_fromUri, args->_toUri, %thePool);
	try
	{
		args->RaiseDiffSummary(e);

		if (e->Cancel)
			return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Diff summary receiver canceled operation");
		else
			return nullptr;
	}
	catch(Exception^ ex)
	{
		return SvnException:: CreateExceptionSvnError("Diff summary receiver", ex);
	}
	finally
	{
		e->Detach(false);
	}

}


bool SvnClient::DiffSummary(SvnTarget^ from, SvnTarget^ to, SvnDiffSummaryArgs^ args, EventHandler<SvnDiffSummaryEventArgs^>^ summaryHandler)
{
	if (!from)
		throw gcnew ArgumentNullException("from");
	else if (!to)
		throw gcnew ArgumentNullException("to");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if (summaryHandler)
		args->DiffSummary += summaryHandler;
	try
	{
		args->_fromUri = pool.AllocString(from->SvnTargetName);
		args->_toUri = pool.AllocString(to->SvnTargetName);

		svn_error_t *r = svn_client_diff_summarize2(
			args->_fromUri,
			from->GetSvnRevision(SvnRevision::Base, SvnRevision::Head)->AllocSvnRevision(%pool),
			args->_toUri,
			to->GetSvnRevision(SvnRevision::Base, SvnRevision::Head)->AllocSvnRevision(%pool),
			(svn_depth_t)args->Depth,
			args->IgnoreAncestry,
			CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
			svn_client_diff_summarize_func_handler,
			(void*)_clientBaton->Handle,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(this, r, from);
	}
	finally
	{
		if (summaryHandler)
			args->DiffSummary -= summaryHandler;

		args->_fromUri = args->_toUri = nullptr;
	}
}

bool SvnClient::GetDiffSummary(SvnTarget^ from, SvnTarget^ to, [Out] Collection<SvnDiffSummaryEventArgs^>^% list)
{
	if (!from)
		throw gcnew ArgumentNullException("from");
	else if (!to)
		throw gcnew ArgumentNullException("to");

	InfoItemCollection<SvnDiffSummaryEventArgs^>^ results = gcnew InfoItemCollection<SvnDiffSummaryEventArgs^>();

	try
	{
		return DiffSummary(from, to, gcnew SvnDiffSummaryArgs(), results->Handler);
	}
	finally
	{
		list = results;
	}
}

bool SvnClient::GetDiffSummary(SvnTarget^ from, SvnTarget^ to, SvnDiffSummaryArgs^ args, [Out] Collection<SvnDiffSummaryEventArgs^>^% list)
{
	if (!from)
		throw gcnew ArgumentNullException("from");
	else if (!to)
		throw gcnew ArgumentNullException("to");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnDiffSummaryEventArgs^>^ results = gcnew InfoItemCollection<SvnDiffSummaryEventArgs^>();

	try
	{
		return DiffSummary(from, to, args, results->Handler);
	}
	finally
	{
		list = results;
	}
}
