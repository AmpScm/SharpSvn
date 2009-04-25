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
#include "Args/Blame.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetBlame(SharpSvn.SvnTarget,SharpSvn.SvnBlameArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnBlameEventArgs>&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetBlame(SharpSvn.SvnTarget,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnBlameEventArgs>&)", MessageId="1#")];

bool SvnClient::Blame(SvnTarget^ target, EventHandler<SvnBlameEventArgs^>^ blameHandler)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!blameHandler)
		throw gcnew ArgumentNullException("blameHandler");

	return Blame(target, gcnew SvnBlameArgs(), blameHandler);
}

static svn_error_t *svn_client_blame_receiver_handler2(void *baton, apr_int64_t line_no, svn_revnum_t revision, const char *author,
													   const char *date, svn_revnum_t merged_revision,  const char *merged_author,
													   const char *merged_date, const char *merged_path, const char *line, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	AprPool thePool(pool, false);

	SvnBlameArgs^ args = dynamic_cast<SvnBlameArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs
	if (!args)
		return nullptr;

	SvnBlameEventArgs^ e = gcnew SvnBlameEventArgs(revision, line_no, author, date, merged_revision, merged_author, merged_date,
		merged_path, line, %thePool);
	try
	{
		args->RaiseBlame(e);

		if (e->Cancel)
			return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Diff summary receiver canceled operation");
		else
			return nullptr;
	}
	catch(Exception^ ex)
	{
		return SvnException::CreateExceptionSvnError("Diff summary receiver", ex);
	}
	finally
	{
		e->Detach(false);
	}
}


bool SvnClient::Blame(SvnTarget^ target, SvnBlameArgs^ args, EventHandler<SvnBlameEventArgs^>^ blameHandler)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if (blameHandler)
		args->Blame += blameHandler;
	try
	{
		svn_diff_file_options_t *options = svn_diff_file_options_create(pool.Handle);

		options->ignore_space = (svn_diff_file_ignore_space_t)args->IgnoreSpacing;
		options->ignore_eol_style = args->IgnoreLineEndings;

		svn_error_t *r = svn_client_blame4(
			pool.AllocString(target->SvnTargetName),
			target->GetSvnRevision(SvnRevision::Working, SvnRevision::Head)->AllocSvnRevision(%pool),
			args->Start->Or(SvnRevision::Zero)->AllocSvnRevision(%pool),
			args->End->Or(SvnRevision::Head)->AllocSvnRevision(%pool),
			options,
			args->IgnoreMimeType,
			args->RetrieveMergedRevisions,
			svn_client_blame_receiver_handler2,
			(void*)_clientBaton->Handle,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(this, r);
	}
	finally
	{
		if (blameHandler)
			args->Blame -= blameHandler;
	}
}

bool SvnClient::GetBlame(SvnTarget^ target, [Out] Collection<SvnBlameEventArgs^>^% list)
{
	if (!target)
		throw gcnew ArgumentNullException("target");

	InfoItemCollection<SvnBlameEventArgs^>^ results = gcnew InfoItemCollection<SvnBlameEventArgs^>();

	try
	{
		return Blame(target, gcnew SvnBlameArgs(), results->Handler);
	}
	finally
	{
		list = results;
	}
}

bool SvnClient::GetBlame(SvnTarget^ target, SvnBlameArgs^ args, [Out] Collection<SvnBlameEventArgs^>^% list)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnBlameEventArgs^>^ results = gcnew InfoItemCollection<SvnBlameEventArgs^>();

	try
	{
		return Blame(target, args, results->Handler);
	}
	finally
	{
		list = results;
	}
}
