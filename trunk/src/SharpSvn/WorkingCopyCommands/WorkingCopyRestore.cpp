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
#include "WorkingCopyArgs/WorkingCopyRestore.h"

#include <private/svn_wc_private.h>

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnWorkingCopyClient::Restore(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	return Restore(path, gcnew SvnWorkingCopyRestoreArgs());
}

bool SvnWorkingCopyClient::Restore(String^ path, SvnWorkingCopyRestoreArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

    svn_error_t *r = SVN_NO_ERROR;
    svn_boolean_t use_commit_times;
    svn_config_t *cfg;

    cfg = CtxHandle->config ? (svn_config_t*)apr_hash_get(CtxHandle->config, SVN_CONFIG_CATEGORY_CONFIG,
                                                          APR_HASH_KEY_STRING) : nullptr;

    SVN_HANDLE(svn_config_get_bool(cfg, &use_commit_times,
                                   SVN_CONFIG_SECTION_MISCELLANY,
                                   SVN_CONFIG_OPTION_USE_COMMIT_TIMES,
                                   FALSE));

    r = svn_wc_restore(
		CtxHandle->wc_ctx,
		pool.AllocAbsoluteDirent(path),
		use_commit_times,
		pool.Handle);

	return args->HandleResult(this, r, path);
}
