// $Id: WorkingCopyEntries.cpp 1432 2010-01-14 11:53:38Z rhuijben $
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
#include "Args/WorkingCopyInstallConflictArgs.h"

#include "UnmanagedStructs.h"
#include <private/svn_wc_private.h>

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnWorkingCopyClient::InstallConflict(String^ targetPath, SvnUriOrigin^ leftSource, SvnUriOrigin^ rightSource, SvnWorkingCopyInstallConflictArgs^ args)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(targetPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "targetPath");
	else if (!leftSource)
		throw gcnew ArgumentNullException("leftSource");
	else if (!rightSource)
		throw gcnew ArgumentNullException("rightSource");
	else if (leftSource->Target->Revision->RevisionType != SvnRevisionType::Number)
		throw gcnew ArgumentException("Revision of left source is not set to a valid revision number", "leftSource");
	else if (rightSource->Target->Revision->RevisionType != SvnRevisionType::Number)
		throw gcnew ArgumentException("Revision of right source is not set to a valid revision number", "rightSource");

	if (!args->TreeConflict)
		throw gcnew InvalidOperationException("This SharpSvn version can only install tree conflicts. Please set "
											  "args.TreeConflict to TRUE for compatibility with future versions that "
											  "might support installing other types of conflicts");

	targetPath = SvnTools::GetNormalizedFullPath(targetPath);

	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);
	const char *path = pool.AllocDirent(targetPath);

	svn_node_kind_t kind;
    SVN_HANDLE(svn_wc_read_kind(&kind, CtxHandle->wc_ctx, path, FALSE, pool.Handle));

	svn_wc_conflict_description2_t *conflict;
	svn_wc_conflict_version_t *left_version, *right_version;

	left_version = svn_wc_conflict_version_create(pool.AllocUri(leftSource->RepositoryRoot),
												  pool.AllocRelpath(rightSource->Uri->MakeRelativeUri(leftSource->RepositoryRoot)->ToString()),
												  (svn_revnum_t)leftSource->Target->Revision->Revision,
												  (svn_node_kind_t)leftSource->NodeKind,
												  pool.Handle);

	right_version = svn_wc_conflict_version_create(pool.AllocUri(rightSource->RepositoryRoot),
												  pool.AllocRelpath(rightSource->Uri->MakeRelativeUri(rightSource->RepositoryRoot)->ToString()),
												  (svn_revnum_t)rightSource->Target->Revision->Revision,
												  (svn_node_kind_t)rightSource->NodeKind,
												  pool.Handle);


    conflict = svn_wc_conflict_description_create_tree2(path,
														kind,
														(svn_wc_operation_t)args->Operation,
														left_version,
														right_version,
														pool.Handle);
	conflict->reason = (svn_wc_conflict_reason_t)args->Reason;
	conflict->action = (svn_wc_conflict_action_t)args->Action;

	SVN_HANDLE(svn_wc__add_tree_conflict(CtxHandle->wc_ctx, conflict, pool.Handle));

	return true;
}
