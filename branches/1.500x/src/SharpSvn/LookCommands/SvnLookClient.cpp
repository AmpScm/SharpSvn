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

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types


using namespace SharpSvn;
using namespace SharpSvn::Implementation;

SvnLookOrigin::SvnLookOrigin(String^ repositoryPath)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!SvnBase::IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");

	_path = SvnTools::GetNormalizedFullPath(repositoryPath);
	_revision = -1;
}

SvnLookOrigin::SvnLookOrigin(String^ repositoryPath, __int64 revision)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!SvnBase::IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");
	else if (revision < 0)
		throw gcnew ArgumentOutOfRangeException("revision", revision, SharpSvnStrings::RevisionMustBeGreaterThanOrEqualToZero);

	_path = SvnTools::GetNormalizedFullPath(repositoryPath);
	_revision = revision;
}

SvnLookOrigin::SvnLookOrigin(String^ repositoryPath, String^ transactionName)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!SvnBase::IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");
	else if (String::IsNullOrEmpty(transactionName))
		throw gcnew ArgumentNullException("transactionName");

	_path = SvnTools::GetNormalizedFullPath(repositoryPath);
	_revision = -1;
	_transactionName = transactionName;
}

SvnLookClient::SvnLookClient()
: _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
}
