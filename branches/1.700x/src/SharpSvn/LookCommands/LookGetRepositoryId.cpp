// $Id: LookGetRevisionProperty.cpp 1699 2011-06-17 13:43:02Z rhuijben $
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
#include "LookArgs/SvnLookRevisionPropertyArgs.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnLookClient::TryGetRepositoryId(SvnLookOrigin^ lookOrigin, [Out] Guid% id)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");

	return TryGetRepositoryId(lookOrigin->RepositoryPath, id);
}

bool SvnLookClient::TryGetRepositoryId(String^ repositoryPath, [Out] Guid% id)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");

	EnsureState(SvnContextState::ConfigLoaded);
	AprPool pool(%_pool);

	id = Guid::Empty;

	svn_error_t* r;
	svn_repos_t* repos;
	if (r = svn_repos_open2(&repos, pool.AllocDirent(repositoryPath), nullptr, pool.Handle))
	{
		svn_error_clear(r);
		return false;
	}

	svn_fs_t* fs = svn_repos_fs(repos);
	const char *uuidStr;

	if (r = svn_fs_get_uuid(fs, &uuidStr, pool.Handle))
	{
		svn_error_clear(r);
		return false;
	}

	id = Guid(Utf8_PtrToString(uuidStr));
	return true;
}

