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

#include "SvnStreamWrapper.h"
#include "ReposArgs/LoadRepository.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace System::IO;
using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnRepositoryClient::LoadRepository(String^ repositoryPath, Stream^ from)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");
	else if (!from)
		throw gcnew ArgumentNullException("from");

	return LoadRepository(repositoryPath, from, gcnew SvnLoadRepositoryArgs());
}

static void
warning_func(void *baton, svn_error_t *err)
{
	/* NOOP: Default handler aborts */
	UNUSED_ALWAYS(baton);
	UNUSED_ALWAYS(err);
}


bool SvnRepositoryClient::LoadRepository(String^ repositoryPath, Stream^ from, SvnLoadRepositoryArgs^ args)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");
	else if (!from)
		throw gcnew ArgumentNullException("from");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");

	EnsureState(SvnContextState::ConfigLoaded);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	svn_repos_t* repos = nullptr;
	svn_error_t* r;

	if (r = svn_repos_open2(&repos, pool.AllocDirent(repositoryPath), nullptr, pool.Handle))
		return args->HandleResult(this, r);

	// Set a simple warning handler (see svnadmin/main.c), otherwise we might abort()
	svn_fs_set_warning_func(svn_repos_fs(repos), warning_func, nullptr);

	MemoryStream^ strResult = gcnew MemoryStream();
	SvnStreamWrapper strmFrom(from, true, false, %pool);
	SvnStreamWrapper strmResponse(strResult, false, true, %pool);

	r = svn_repos_load_fs2(
		repos,
		strmFrom.Handle,
		strmResponse.Handle,
		(svn_repos_load_uuid)args->LoadIdType,
		args->ImportParent ? pool.AllocRelpath(args->ImportParent) : nullptr,
		args->RunPreCommitHook,
		args->RunPostCommitHook,
		CtxHandle->cancel_func,
		CtxHandle->cancel_baton,
		pool.Handle);

	return args->HandleResult(this, r);

	// Pool close will close all handles
}