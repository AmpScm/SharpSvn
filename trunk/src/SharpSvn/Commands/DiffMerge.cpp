// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/DiffMerge.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::DiffMerge(String^ targetPath, SvnTarget^ mergeFrom, SvnTarget^ mergeTo)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!mergeFrom)
		throw gcnew ArgumentNullException("mergeFrom");
	else if(!mergeFrom)
		throw gcnew ArgumentNullException("mergeTo");

	return DiffMerge(targetPath, mergeFrom, mergeTo, gcnew SvnDiffMergeArgs());
}

bool SvnClient::DiffMerge(String^ targetPath, SvnTarget^ mergeFrom, SvnTarget^ mergeTo, SvnDiffMergeArgs^ args)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!mergeFrom)
		throw gcnew ArgumentNullException("mergeFrom");
	else if(!mergeFrom)
		throw gcnew ArgumentNullException("mergeTo");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	apr_array_header_t *merge_options = nullptr;

	svn_opt_revision_t mergeFromRev = mergeFrom->Revision->ToSvnRevision();
	svn_opt_revision_t mergeToRev = mergeTo->Revision->ToSvnRevision();

	svn_error_t *r = svn_client_merge3(
		pool.AllocString(mergeFrom->TargetName),
		&mergeFromRev,
		pool.AllocString(mergeTo->TargetName),
		&mergeToRev,
		pool.AllocPath(targetPath),
		(svn_depth_t)args->Depth,
		args->IgnoreAncestry,
		args->Force,
		args->RecordOnly,
		args->DryRun,
		merge_options,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}
