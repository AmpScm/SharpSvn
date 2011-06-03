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
#include "Args/GetWorkingCopyVersion.h"
#include "Args/SvnWorkingCopyVersion.h"

#include "UnmanagedStructs.h"

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnWorkingCopyClient.#GetVersion(System.String,SharpSvn.SvnWorkingCopyVersion&)", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnWorkingCopyClient.#GetVersion(System.String,SharpSvn.SvnGetWorkingCopyVersionArgs,SharpSvn.SvnWorkingCopyVersion&)", MessageId="2#")];

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;


bool SvnWorkingCopyClient::GetVersion(String^ targetPath, [Out] SvnWorkingCopyVersion^% version)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");

	return GetVersion(targetPath, gcnew SvnGetWorkingCopyVersionArgs(), version);
}

bool SvnWorkingCopyClient::GetVersion(String^ targetPath, SvnGetWorkingCopyVersionArgs^ args, [Out] SvnWorkingCopyVersion^% version)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_wc_revision_status_t *result = nullptr;

	svn_error_t *r = svn_wc_revision_status(
		&result,
		pool.AllocPath(targetPath),
		args->Trail ? pool.AllocString(args->Trail) : nullptr,
		args->UseCommittedRevisions,
		CtxHandle->cancel_func,
		CtxHandle->cancel_baton,
		pool.Handle);

	if (result)
		version = gcnew SvnWorkingCopyVersion(result->min_rev, result->max_rev, result->switched != 0, result->modified != 0, result->sparse_checkout != 0);
	else
		version = nullptr;

	return args->HandleResult(this, r, targetPath);
}
