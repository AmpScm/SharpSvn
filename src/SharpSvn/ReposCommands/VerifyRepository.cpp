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
#include "ReposArgs/VerifyRepository.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn;
using namespace SharpSvn::Implementation;


bool SvnRepositoryClient::Verify(String^ repositoryPath)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");

	return Verify(repositoryPath, gcnew SvnVerifyRepositoryArgs());
}

bool SvnRepositoryClient::Verify(String^ repositoryPath, SvnVerifyRepositoryArgs^ args)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	svn_repos_t* repos = nullptr;

	SVN_HANDLE(svn_repos_open2(&repos, pool.AllocDirent(repositoryPath), nullptr, pool.Handle));

	svn_error_t* r = svn_repos_verify_fs2(
		repos,
		(svn_revnum_t)args->StartRevision,
		(svn_revnum_t)args->EndRevision,
		repos_notify_func,
		_clientBaton->Handle,
		CtxHandle->cancel_func,
		CtxHandle->cancel_baton,
		pool.Handle);

	return args->HandleResult(this, r);
}