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
#include "Args/WorkingCopyState.h"

#include "UnmanagedStructs.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnWorkingCopyClient.#GetState(System.String,SharpSvn.SvnWorkingCopyState&)", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnWorkingCopyClient.#GetState(System.String,SharpSvn.SvnWorkingCopyStateArgs,SharpSvn.SvnWorkingCopyState&)")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnWorkingCopyClient.#GetState(System.String,SharpSvn.SvnWorkingCopyStateArgs,SharpSvn.SvnWorkingCopyState&)", MessageId="2#")];

bool SvnWorkingCopyClient::GetState(String^ targetPath, [Out] SvnWorkingCopyState^% result)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if (!IsNotUri(targetPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "targetPath");

	return GetState(targetPath, gcnew SvnWorkingCopyStateArgs(), result);
}

bool SvnWorkingCopyClient::GetState(String^ targetPath, SvnWorkingCopyStateArgs^ args, [Out] SvnWorkingCopyState^% result)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(targetPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "targetPath");

	EnsureState(SvnContextState::ConfigLoaded);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	const char* pPath = pool.AllocPath(targetPath);
	svn_wc_adm_access_t* acc = nullptr;

	svn_error_t* r = svn_wc_adm_probe_open3(&acc, nullptr, pPath, false, 1, CtxHandle->cancel_func, CtxHandle->cancel_baton, pool.Handle);

	if (r)
		return args->HandleResult(this, r, targetPath);

	try
	{
		svn_boolean_t pIsBinary = 0;

		r = svn_wc_has_binary_prop(&pIsBinary, pPath, acc, pool.Handle);
		if (r)
			return args->HandleResult(this, r, targetPath);

		result = gcnew SvnWorkingCopyState(!pIsBinary);
	}
	finally
	{
		svn_wc_adm_close2(acc, pool.Handle);
	}

	return true;
}
