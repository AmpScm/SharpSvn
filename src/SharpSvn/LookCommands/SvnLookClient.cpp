// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

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
