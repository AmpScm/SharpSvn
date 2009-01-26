// $Id$
//
// Copyright 2007-2008 The SharpSvn Project
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


