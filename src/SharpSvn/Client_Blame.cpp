// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetBlame(SharpSvn.SvnTarget,SharpSvn.SvnBlameArgs,System.Collections.Generic.IList`1<SharpSvn.SvnBlameEventArgs>&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetBlame(SharpSvn.SvnTarget,System.Collections.Generic.IList`1<SharpSvn.SvnBlameEventArgs>&):System.Boolean", MessageId="1#")];

bool SvnClient::Blame(SvnTarget^ target, EventHandler<SvnBlameEventArgs^>^ blameHandler)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!blameHandler)
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
	if(args)
	{
		SvnBlameEventArgs^ e = gcnew SvnBlameEventArgs(revision, line_no, author, date, merged_revision, merged_author, merged_date, 
													   merged_path, line, %thePool);
		try
		{
			args->OnBlameHandler(e);

			if(e->Cancel)
				return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Diff summary receiver canceled operation");
		}
		catch(Exception^ e)
		{
			return SvnException::CreateExceptionSvnError("Diff summary receiver", e);
		}
		finally
		{
			e->Detach(false);
		}
	}

	return nullptr;
}


bool SvnClient::Blame(SvnTarget^ target, SvnBlameArgs^ args, EventHandler<SvnBlameEventArgs^>^ blameHandler)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if(blameHandler)
		args->BlameHandler += blameHandler;
	try
	{
		svn_diff_file_options_t *options = svn_diff_file_options_create(pool.Handle);

		options->ignore_space = (svn_diff_file_ignore_space_t)args->IgnoreSpacing;
		options->ignore_eol_style = args->IgnoreLineEndings;

		svn_opt_revision_t pegRev = target->GetSvnRevision(SvnRevision::Working, SvnRevision::Head);

		svn_error_t *r = svn_client_blame4(
			pool.AllocString(target->TargetName),
			&pegRev,
			args->Start->AllocSvnRevision(%pool),
			args->End->AllocSvnRevision(%pool),
			options,
			args->IgnoreMimeType,
			args->IncludeMergedRevisions,
			svn_client_blame_receiver_handler2,
			(void*)_clientBatton->Handle,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(this, r);
	}
	finally
	{
		if(blameHandler)
			args->BlameHandler -= blameHandler;
	}
}

bool SvnClient::GetBlame(SvnTarget^ target, [Out] IList<SvnBlameEventArgs^>^% list)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	InfoItemCollection<SvnBlameEventArgs^>^ results = gcnew InfoItemCollection<SvnBlameEventArgs^>();

	try
	{
		return Blame(target, gcnew SvnBlameArgs(), results->Handler);
	}
	finally
	{
		list = safe_cast<IList<SvnBlameEventArgs^>^>(results);
	}
}

bool SvnClient::GetBlame(SvnTarget^ target, SvnBlameArgs^ args, [Out] IList<SvnBlameEventArgs^>^% list)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnBlameEventArgs^>^ results = gcnew InfoItemCollection<SvnBlameEventArgs^>();

	try
	{
		return Blame(target, args, results->Handler);
	}
	finally
	{
		list = safe_cast<IList<SvnBlameEventArgs^>^>(results);
	}
}
