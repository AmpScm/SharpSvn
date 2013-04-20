#include "stdafx.h"
#include "GitClient.h"
#include "GitClone.h"

#include "Plumbing/GitRepository.h"
#include "Plumbing/GitIndex.h"

using namespace SharpGit;
using namespace SharpGit::Plumbing;

const git_clone_options * GitCloneArgs::MakeCloneOptions(GitPool ^pool)
{
	if (! pool)
		throw gcnew ArgumentNullException("pool");

	git_clone_options *opts = (git_clone_options *)pool->Alloc(sizeof(*opts));

	opts->version = GIT_CLONE_OPTIONS_VERSION;
	opts->bare = CreateBareRepository;

	const git_checkout_opts *coo = MakeCheckOutOptions(pool);
	opts->checkout_opts = *coo;

	return opts;
}

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

	GitPool pool(_pool);

	return CloneInternal(svn_uri_canonicalize(pool.AllocString(remoteRepository->AbsoluteUri), pool.Handle), path, args, %pool);
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

	GitPool pool(_pool);
	return CloneInternal(pool.AllocDirent(path), path, args, %pool);
}

bool GitClient::CloneInternal(const char *rawRepository, String ^path, GitCloneArgs ^args, GitPool ^pool)
{
	git_repository *repository;

	int r;

	if (args->Synchronous)
	{
		r = git_clone(&repository, rawRepository, pool->AllocDirent(path), args->MakeCloneOptions(pool));
	}
	else
		throw gcnew NotImplementedException();


	return args->HandleGitError(this, r);
}