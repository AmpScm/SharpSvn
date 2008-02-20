// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#include "SvnAll.h"
#include "SvnClientArgs_Admin.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types


using namespace SharpSvn;
using namespace SharpSvn::Implementation;


[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnRepositoryClient.DeleteRepository(System.String,SharpSvn.SvnDeleteRepositoryArgs):System.Boolean")];
[module: SuppressMessage("Microsoft.Naming", "CA1709:IdentifiersShouldBeCasedCorrectly", Scope="member", Target="SharpSvn.SvnRepositoryFileSystem.#FsFs", MessageId="Fs")];

SvnRepositoryClient::SvnRepositoryClient()
: _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
}

SvnRepositoryClient::~SvnRepositoryClient()
{
}

String^ SvnRepositoryClient::FindRepositoryRoot(Uri^ repositoryUri)
{
	if (!repositoryUri)
		throw gcnew ArgumentNullException("repositoryUri");

	EnsureState(SvnContextState::ConfigLoaded);

	AprPool pool(%_pool);

	const char* root = svn_repos_find_root_path(
		pool.AllocCanonical(repositoryUri->ToString()),
		pool.Handle);

	return root ? Utf8_PtrToString(root) : nullptr;
}

void SvnRepositoryClient::CreateRepository(String^ repositoryPath)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");

	CreateRepository(repositoryPath, gcnew SvnCreateRepositoryArgs());
}

bool SvnRepositoryClient::CreateRepository(String^ repositoryPath, SvnCreateRepositoryArgs^ args)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);

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
		// Use 1.1-1.3 format
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
		// 

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

void SvnRepositoryClient::DeleteRepository(String^ repositoryPath)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");

	DeleteRepository(repositoryPath, gcnew SvnDeleteRepositoryArgs());
}

bool SvnRepositoryClient::DeleteRepository(String^ repositoryPath, SvnDeleteRepositoryArgs^ args)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	AprPool pool(%_pool);

	svn_error_t* r = svn_repos_delete(
		pool.AllocPath(repositoryPath),
		pool.Handle);

	return args->HandleResult(this, r);
}

void SvnRepositoryClient::RecoverRepository(String^ repositoryPath)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");

	RecoverRepository(repositoryPath, gcnew SvnRecoverRepositoryArgs());
}

bool SvnRepositoryClient::RecoverRepository(String^ repositoryPath, SvnRecoverRepositoryArgs^ args)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	AprPool pool(%_pool);

	svn_error_t* r = svn_repos_recover3(
		pool.AllocPath(repositoryPath),
		args->NonBlocking,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		pool.Handle);


	return args->HandleResult(this, r);
}
