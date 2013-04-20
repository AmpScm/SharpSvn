#include "stdafx.h"
#include "GitClient.h"
#include "GitClone.h"

#include "Plumbing/GitRepository.h"
#include "Plumbing/GitIndex.h"

using namespace SharpGit;
using namespace SharpGit::Plumbing;

bool GitClient::Clone(Uri ^remoteRepository, String ^path)
{
	return Clone(remoteRepository, path, gcnew GitCloneArgs());
}

bool GitClient::Clone(String ^localRepository, String ^path)
{
	return Clone(localRepository, path, gcnew GitCloneArgs());
}

bool GitClient::Clone(Uri ^remoteRepository, String ^path, GitCloneArgs ^args)
{
	if (! remoteRepository)
		throw gcnew ArgumentNullException("remoteRepository");
	else if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (! args)
		throw gcnew ArgumentNullException("args");

	return CloneInternal(remoteRepository->AbsoluteUri, path, args);
}

bool GitClient::Clone(String ^localRepository, String ^path, GitCloneArgs ^args)
{
	if (String::IsNullOrEmpty(localRepository))
		throw gcnew ArgumentNullException("remoteRepository");
	else if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (! args)
		throw gcnew ArgumentNullException("args");

	if (localRepository->LastIndexOf(':') > 1)
		throw gcnew ArgumentOutOfRangeException("Use Clone(Uri,...) to pass urls", "localRepository");

	return CloneInternal(localRepository, path, args);
}

bool GitClient::CloneInternal(String ^rawRepository, String ^path, GitCloneArgs ^args)
{
	//git_clone();
	return false;
}