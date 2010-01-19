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

	ArgsStore store(this, args);
	AprPool pool(%_pool);
	svn_wc_adm_access_t *adm_access;
	const char *path = pool.AllocPath(targetPath);

	SVN_HANDLE(svn_wc_adm_probe_open3(&adm_access,
									  NULL, 
									  path, 
									  TRUE, 
									  1, 
									  CtxHandle->cancel_func, CtxHandle->cancel_baton,
									  pool.Handle));

	svn_wc_status2_t *status;
	SVN_HANDLE(svn_wc_status2(&status, path, adm_access, pool.Handle));

	svn_wc_conflict_description_t *conflict;
	svn_wc_conflict_version_t *left_version, *right_version;

	left_version = svn_wc_conflict_version_create(pool.AllocCanonical(leftSource->RepositoryRoot),
												  pool.AllocCanonical(rightSource->Uri->MakeRelativeUri(leftSource->RepositoryRoot)->ToString()),
												  (svn_revnum_t)leftSource->Target->Revision->Revision,
												  (svn_node_kind_t)leftSource->NodeKind,
												  pool.Handle);

	right_version = svn_wc_conflict_version_create(pool.AllocCanonical(rightSource->RepositoryRoot),
												  pool.AllocCanonical(rightSource->Uri->MakeRelativeUri(rightSource->RepositoryRoot)->ToString()),
												  (svn_revnum_t)rightSource->Target->Revision->Revision,
												  (svn_node_kind_t)rightSource->NodeKind,
												  pool.Handle);


	conflict = svn_wc_conflict_description_create_tree(path,
													   adm_access,
													   status->entry->kind,
													   (svn_wc_operation_t)args->Operation,
													   left_version,
													   right_version,
													   pool.Handle);
	conflict->reason = (svn_wc_conflict_reason_t)args->Reason;
	conflict->action = (svn_wc_conflict_action_t)args->Action;

	SVN_HANDLE(svn_wc__add_tree_conflict(conflict, adm_access, pool.Handle));

	SVN_HANDLE(svn_wc_adm_close2(adm_access, pool.Handle));

	return true;
}
