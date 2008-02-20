// $Id: SvnRepositoryClient.cpp 300 2008-02-20 11:16:38Z bhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "ReposArgs/DeleteRepository.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnRepositoryClient.DeleteRepository(System.String,SharpSvn.SvnDeleteRepositoryArgs):System.Boolean")];

bool SvnRepositoryClient::DeleteRepository(String^ repositoryPath)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");

	return DeleteRepository(repositoryPath, gcnew SvnDeleteRepositoryArgs());
}

bool SvnRepositoryClient::DeleteRepository(String^ repositoryPath, SvnDeleteRepositoryArgs^ args)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_error_t* r = svn_repos_delete(
		pool.AllocPath(repositoryPath),
		pool.Handle);

	return args->HandleResult(this, r);
}
