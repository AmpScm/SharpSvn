// Copyright 2008-2025 The SharpSvn Project
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

#include "ReposArgs/DeleteRepository.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnRepositoryClient.DeleteRepository(System.String,SharpSvn.SvnDeleteRepositoryArgs):System.Boolean")];

bool SvnRepositoryClient::DeleteRepository(String^ repositoryPath)
{
    if (String::IsNullOrEmpty(repositoryPath))
        throw gcnew ArgumentNullException("repositoryPath");
    else if (!IsNotUri(repositoryPath))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

    return DeleteRepository(repositoryPath, gcnew SvnDeleteRepositoryArgs());
}

bool SvnRepositoryClient::DeleteRepository(String^ repositoryPath, SvnDeleteRepositoryArgs^ args)
{
    if (String::IsNullOrEmpty(repositoryPath))
        throw gcnew ArgumentNullException("repositoryPath");
    else if (!IsNotUri(repositoryPath))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");
    else if (!args)
        throw gcnew ArgumentNullException("args");

    EnsureState(SvnContextState::ConfigLoaded);
    AprPool pool(%_pool);
    ArgsStore store(this, args, %pool);

    svn_error_t* r = svn_repos_delete(
        pool.AllocDirent(repositoryPath),
        pool.Handle);

    return args->HandleResult(this, r);
}
