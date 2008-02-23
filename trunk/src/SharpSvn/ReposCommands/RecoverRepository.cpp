// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "ReposArgs/RecoverRepository.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnRepositoryClient::RecoverRepository(String^ repositoryPath)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");

	return RecoverRepository(repositoryPath, gcnew SvnRecoverRepositoryArgs());
}

bool SvnRepositoryClient::RecoverRepository(String^ repositoryPath, SvnRecoverRepositoryArgs^ args)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
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