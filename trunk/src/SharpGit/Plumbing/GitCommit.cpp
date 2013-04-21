#include "stdafx.h"

#include "GitCommit.h"
#include "GitRepository.h"

using namespace SharpGit::Plumbing;

struct git_commit {};

bool GitCommit::IsDisposed::get()
{
	return !_commit || _repository->IsDisposed;
}
