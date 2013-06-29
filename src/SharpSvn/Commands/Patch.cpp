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
#include "Args/Patch.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;

bool SvnClient::Patch(String^ patchFile, String^ targetDir)
{
	if (String::IsNullOrEmpty(patchFile))
		throw gcnew ArgumentNullException("patchFile");
	else if (!IsNotUri(patchFile))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "patchFile");
	else if (String::IsNullOrEmpty(targetDir))
		throw gcnew ArgumentNullException("targetDir");
	else if (!IsNotUri(targetDir))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "targetDir");

	return SvnClient::Patch(patchFile, targetDir, gcnew SvnPatchArgs());
}

static svn_error_t * __cdecl
patch_func(void *baton, svn_boolean_t *filtered, const char *canon_path_from_patchfile, const char *patch_abspath, const char *reject_abspath, apr_pool_t *scratch_pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	AprPool aprPool(scratch_pool, false);
	SvnPatchArgs^ args = dynamic_cast<SvnPatchArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs

	if (!args)
		return nullptr;

	SvnPatchFilterEventArgs^ e = gcnew SvnPatchFilterEventArgs(canon_path_from_patchfile, patch_abspath, reject_abspath, client, %aprPool);

	try
	{
		args->OnFilter(e);

		if (e->Cancel)
			return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Patch filter canceled operation");
		else
			return nullptr;

		*filtered = e->Filtered;
	}
	catch(Exception^ ex)
	{
		return SvnException::CreateExceptionSvnError("Patch filter", ex);
	}
	finally
	{
		e->Detach(false);
	}

}

bool SvnClient::Patch(String^ patchFile, String^ targetDir, SvnPatchArgs^ args)
{
	if (String::IsNullOrEmpty(patchFile))
		throw gcnew ArgumentNullException("patchFile");
	else if (!IsNotUri(patchFile))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "patchFile");
	else if (String::IsNullOrEmpty(targetDir))
		throw gcnew ArgumentNullException("targetDir");
	else if (!IsNotUri(targetDir))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "targetDir");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);

	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	svn_error_t* r = svn_client_patch(
						pool.AllocAbsoluteDirent(patchFile),
						pool.AllocAbsoluteDirent(targetDir),
						args->DryRun,
						args->StripCount,
						args->Reverse,
						args->IgnoreWhitespace,
						TRUE,
						patch_func, (void*)_clientBaton->Handle,
						CtxHandle,
						pool.Handle);

	return args->HandleResult(this, r, targetDir);
}
