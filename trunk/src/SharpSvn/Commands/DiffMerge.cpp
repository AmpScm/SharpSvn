// $Id$
//
// Copyright 2008-2009 The SharpSvn Project
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
#include "Args/DiffMerge.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::DiffMerge(String^ targetPath, SvnTarget^ mergeFrom, SvnTarget^ mergeTo)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if (!mergeFrom)
		throw gcnew ArgumentNullException("mergeFrom");
	else if (!mergeFrom)
		throw gcnew ArgumentNullException("mergeTo");

	return DiffMerge(targetPath, mergeFrom, mergeTo, gcnew SvnDiffMergeArgs());
}

bool SvnClient::DiffMerge(String^ targetPath, SvnTarget^ mergeFrom, SvnTarget^ mergeTo, SvnDiffMergeArgs^ args)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if (!mergeFrom)
		throw gcnew ArgumentNullException("mergeFrom");
	else if (!mergeFrom)
		throw gcnew ArgumentNullException("mergeTo");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	if (!mergeFrom->Revision->IsExplicit && !dynamic_cast<SvnUriTarget^>(mergeFrom))
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "mergeFrom");
	else if (!mergeTo->Revision->IsExplicit && !dynamic_cast<SvnUriTarget^>(mergeTo))
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "mergeTo");

	EnsureState(SvnContextState::AuthorizationInitialized);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	apr_array_header_t *merge_options = nullptr;

	svn_opt_revision_t mergeFromRev = mergeFrom->GetSvnRevision(SvnRevision::None, SvnRevision::Head)->ToSvnRevision();
	svn_opt_revision_t mergeToRev = mergeTo->GetSvnRevision(SvnRevision::None, SvnRevision::Head)->ToSvnRevision();

	svn_error_t *r = svn_client_merge3(
		pool.AllocString(mergeFrom->SvnTargetName),
		&mergeFromRev,
		pool.AllocString(mergeTo->SvnTargetName),
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

	return args->HandleResult(this, r, targetPath);
}
