#include "stdafx.h"
#include "GitClient.h"
#include "GitAdd.h"

#include "Plumbing/GitRepository.h"
#include "Plumbing/GitIndex.h"

using namespace SharpGit;
using namespace SharpGit::Plumbing;

void GitClient::AssertNotBare(GitRepository ^repository)
{
	if (! repository)
		throw gcnew ArgumentNullException("repository");

	if (repository->IsBare)
		throw gcnew InvalidOperationException("Not valid on a bare repository");
}

void GitClient::AssertNotBare(GitRepository %repository)
{
	AssertNotBare(%repository);
}

bool GitClient::Add(String ^path)
{
	return Add(path, gcnew GitAddArgs());
}

bool GitClient::Add(String ^path, GitAddArgs ^args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (! args)
		throw gcnew ArgumentNullException("args");

	GitRepository repo;
	
	if (!repo.Locate(path, args))
		return false;

	AssertNotBare(repo);

	GitPool pool(_pool);

	return repo.Index->Add(repo.MakeRelpath(path, %pool), args, %pool)
			&& repo.Index->Write(args);
}

