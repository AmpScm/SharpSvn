// $Id: SvnRepositoryClient.cpp 300 2008-02-20 11:16:38Z bhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

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
	else if (!from)
		throw gcnew ArgumentNullException("from");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_repos_t* repos = nullptr;
	svn_error_t* r;
	
	if(r = svn_repos_open(&repos, pool.AllocPath(repositoryPath), pool.Handle))
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
		args->ImportParent ? pool.AllocCanonical(args->ImportParent) : nullptr,
		args->RunPreCommitHook,
		args->RunPostCommitHook,
		nullptr,
		nullptr,
		pool.Handle);

	return args->HandleResult(this, r);

	// Pool close will close all handles
}