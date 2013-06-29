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

#include "ReposArgs/SetRepositoryId.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace System::IO;
using namespace SharpSvn;

bool SvnRepositoryClient::SetRepositoryId(String^ repositoryPath, Guid id)
{
	return SetRepositoryId(repositoryPath, id, gcnew SvnSetRepositoryIdArgs());
}

bool SvnRepositoryClient::SetRepositoryId(String^ repositoryPath, Guid id, SvnSetRepositoryIdArgs ^args)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	EnsureState(SvnContextState::ConfigLoaded);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	svn_repos_t* repos = nullptr;
	svn_error_t* r;

	if (r = svn_repos_open2(&repos, pool.AllocDirent(repositoryPath), nullptr, pool.Handle))
		return args->HandleResult(this, r);

	r = svn_fs_set_uuid(svn_repos_fs(repos), pool.AllocString(id.ToString()), pool.Handle);

	return args->HandleResult(this, r);
}
