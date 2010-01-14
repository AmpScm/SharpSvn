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
	ArgsStore store(this, args);

	svn_repos_t* result;

	AprPool pool(%_pool);

	apr_hash_t *fs_config = apr_hash_make(pool.Handle);

	apr_hash_set(
		fs_config,
		SVN_FS_CONFIG_BDB_TXN_NOSYNC,
		APR_HASH_KEY_STRING,
		(args->BerkeleyDBNoFSyncAtCommit ? "1" : "0"));

	apr_hash_set(
		fs_config,
		SVN_FS_CONFIG_BDB_LOG_AUTOREMOVE,
		APR_HASH_KEY_STRING,
		(args->BerkeleyDBKeepTransactionLogs ? "0" : "1"));


	switch(args->RepositoryType)
	{
	case SvnRepositoryFileSystem::FsFs:
		apr_hash_set(
			fs_config,
			SVN_FS_CONFIG_FS_TYPE,
			APR_HASH_KEY_STRING,
			SVN_FS_TYPE_FSFS);
		break;
	case SvnRepositoryFileSystem::BerkeleyDB:
		apr_hash_set(
			fs_config,
			SVN_FS_CONFIG_FS_TYPE,
			APR_HASH_KEY_STRING,
			SVN_FS_TYPE_BDB);
		break;
	default:
		break;
	}

	switch(args->RepositoryCompatibility)
	{
	case SvnRepositoryCompatibility::Subversion10:
		// Use 1.0-1.3 format
		apr_hash_set(
			fs_config,
			SVN_FS_CONFIG_PRE_1_4_COMPATIBLE,
			APR_HASH_KEY_STRING,
			"1");
		// fall through
	case SvnRepositoryCompatibility::Subversion14:
		// Use 1.4 format
		apr_hash_set(
			fs_config,
			SVN_FS_CONFIG_PRE_1_5_COMPATIBLE,
			APR_HASH_KEY_STRING,
			"1");
		// fall through
	case SvnRepositoryCompatibility::Subversion15:
		// Use 1.5 format
		apr_hash_set(
			fs_config,
			SVN_FS_CONFIG_PRE_1_6_COMPATIBLE,
			APR_HASH_KEY_STRING,
			"1");
		// fall through
	case SvnRepositoryCompatibility::Subversion16:
		// .....

		break;
	case SvnRepositoryCompatibility::Default:
	default:
		// Use default format
		break;
	}

	svn_error_t* r = svn_repos_create(
		&result,
		pool.AllocPath(repositoryPath),
		nullptr,
		nullptr,
		CtxHandle->config,
		fs_config,
		pool.Handle);

	return args->HandleResult(this, r);
}