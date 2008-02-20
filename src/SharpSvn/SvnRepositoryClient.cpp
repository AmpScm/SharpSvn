// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#include "SvnAll.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types


using namespace SharpSvn;
using namespace SharpSvn::Implementation;



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
		pool.AllocCanonical(repositoryUri),
		pool.Handle);

	return root ? Utf8_PtrToString(root) : nullptr;
}


