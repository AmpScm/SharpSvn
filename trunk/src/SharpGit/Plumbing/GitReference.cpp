#include "stdafx.h"

#include "GitRepository.h"
#include "GitReference.h"

using namespace SharpGit;
using namespace SharpGit::Plumbing;

struct git_reference {};

bool GitReference::IsDisposed::get()
{
	return !_reference && !_repository->IsDisposed;
}

bool GitReference::Delete()
{
	return Delete(gcnew GitNoArgs());
}

bool GitReference::Delete(GitArgs ^args)
{
	if (!args)
		throw gcnew ArgumentNullException("args");

	int r = git_reference_delete(Handle);

	try
	{
		return args->HandleGitError(this, r);
	}
	finally
	{
		delete this;
	}
}
