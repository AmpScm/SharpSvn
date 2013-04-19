#include "stdafx.h"
#include "GitClient.h"
#include "GitStatus.h"
#include "Plumbing/GitRepository.h"

using namespace SharpGit;
using namespace SharpGit::Plumbing;

bool GitClient::Status(String ^path, EventHandler<GitStatusEventArgs^>^ status)
{
	return GitClient::Status(path, gcnew GitStatusArgs(), status);
}

bool GitClient::Status(String ^path, GitStatusArgs ^args, EventHandler<GitStatusEventArgs^>^ status)
{
	GitRepository repo;
	
	if (!repo.Locate(path, args))
		return false;
	
	return false;
}