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

#include "ReposArgs/CreateRepository.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnRepositoryClient::CreateRepository(String^ repositoryPath)
{
    if (String::IsNullOrEmpty(repositoryPath))
        throw gcnew ArgumentNullException("repositoryPath");
    else if (!IsNotUri(repositoryPath))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

    return CreateRepository(repositoryPath, gcnew SvnCreateRepositoryArgs());
}

bool SvnRepositoryClient::CreateRepository(String^ repositoryPath, SvnCreateRepositoryArgs^ args)
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

    svn_repos_t* result;

    apr_hash_t *fs_config = apr_hash_make(pool.Handle);

    svn_hash_sets(
        fs_config,
        SVN_FS_CONFIG_BDB_TXN_NOSYNC,
        (args->BerkeleyDBNoFSyncAtCommit ? "1" : "0"));

    svn_hash_sets(
        fs_config,
        SVN_FS_CONFIG_BDB_LOG_AUTOREMOVE,
        (args->BerkeleyDBKeepTransactionLogs ? "0" : "1"));


    switch(args->RepositoryType)
    {
    case SvnRepositoryFileSystem::FsFs:
        svn_hash_sets(
            fs_config,
            SVN_FS_CONFIG_FS_TYPE,
            SVN_FS_TYPE_FSFS);
        break;
    case SvnRepositoryFileSystem::BerkeleyDB:
        svn_hash_sets(
            fs_config,
            SVN_FS_CONFIG_FS_TYPE,
            SVN_FS_TYPE_BDB);
        break;
    default:
        break;
    }

    switch(args->RepositoryCompatibility)
    {
    case SvnRepositoryCompatibility::Subversion10:
        // Use 1.0-1.3 format
        svn_hash_sets(
            fs_config,
            SVN_FS_CONFIG_PRE_1_4_COMPATIBLE,
            "1");
        // fall through
    case SvnRepositoryCompatibility::Subversion14:
        // Use 1.4 format
        svn_hash_sets(
            fs_config,
            SVN_FS_CONFIG_PRE_1_5_COMPATIBLE,
            "1");
        // fall through
    case SvnRepositoryCompatibility::Subversion15:
        // Use 1.5 format
        svn_hash_sets(
            fs_config,
            SVN_FS_CONFIG_PRE_1_6_COMPATIBLE,
            "1");
        // fall through
    case SvnRepositoryCompatibility::Subversion16:
        // fall through
    case SvnRepositoryCompatibility::Subversion17:
        // .....
        svn_hash_sets(
            fs_config,
            SVN_FS_CONFIG_PRE_1_8_COMPATIBLE,
            "1");
        break;
    case SvnRepositoryCompatibility::Subversion18:
        svn_hash_sets(fs_config,
                      SVN_FS_CONFIG_COMPATIBLE_VERSION,
                      "1.8");
        break;
        // fall through
    case SvnRepositoryCompatibility::Subversion19:
    case SvnRepositoryCompatibility::Default:
    default:
        // Use default format
        break;
    }

    svn_error_t* r = svn_repos_create(
        &result,
        pool.AllocDirent(repositoryPath),
        nullptr,
        nullptr,
        CtxHandle->config,
        fs_config,
        pool.Handle);

    if (!r && args->RepositoryUuid.HasValue)
    {
        svn_fs_t *fs = svn_repos_fs(result);
        r = svn_fs_set_uuid(fs, pool.AllocString(args->RepositoryUuid.ToString()), pool.Handle);
    }

    return args->HandleResult(this, r);
}
