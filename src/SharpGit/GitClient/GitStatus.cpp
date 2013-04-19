#include "stdafx.h"
#include "GitClient.h"
#include "GitStatus.h"
#include "Plumbing/GitRepository.h"

using namespace SharpGit;
using namespace SharpGit::Plumbing;

GitStatusEventArgs::GitStatusEventArgs(const char *path, unsigned status, GitStatusArgs ^args, Implementation::GitPool ^pool)
{
}

const git_status_options* GitStatusArgs::MakeOptions(String^ path, Implementation::GitPool ^pool)
{
	git_status_options *opts = (git_status_options *)pool->Alloc(sizeof(*opts));

	opts->flags = 0;
	opts->show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;

	return opts;
}

bool GitClient::Status(String ^path, EventHandler<GitStatusEventArgs^>^ status)
{
	return GitClient::Status(path, gcnew GitStatusArgs(), status);
}

bool GitClient::Status(String ^path, GitStatusArgs ^args, EventHandler<GitStatusEventArgs^>^ status)
{
	GitRepository repo;
	
	if (!repo.Locate(path, args))
		return false;

	return repo.Status(path, args, status);
}