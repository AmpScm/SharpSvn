#include "stdafx.h"
#include "GitClient.h"
#include "GitCommitCmd.h"

#include "Plumbing/GitRepository.h"
#include "Plumbing/GitIndex.h"

using namespace SharpGit;
using namespace SharpGit::Plumbing;

bool GitClient::Commit(String ^path)
{
	GitId ^ignored;

	return Commit(path, ignored);
}

bool GitClient::Commit(System::Collections::Generic::ICollection<String^> ^paths)
{
	GitId ^ignored;

	return Commit(paths, ignored);
}

bool GitClient::Commit(String ^path, GitCommitArgs ^args)
{
	GitId ^ignored;

	return Commit(path, args, ignored);
}

bool GitClient::Commit(System::Collections::Generic::ICollection<String^> ^paths, GitCommitArgs ^args)
{
	GitId ^ignored;

	return Commit(paths, args, ignored);
}

bool GitClient::Commit(String ^path, [Out] GitId ^%commitId)
{
	return Commit(path, gcnew GitCommitArgs(), commitId);
}

bool GitClient::Commit(String ^path, GitCommitArgs ^args, [Out] GitId ^%commitId)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	array<String^>^ list = gcnew array<String^>(1);
	list[0] = path;

	return Commit(safe_cast<System::Collections::Generic::ICollection<String^>^>(list), args, commitId);
}

bool GitClient::Commit(System::Collections::Generic::ICollection<String^> ^paths, [Out] GitId ^%commitId)
{
	return Commit(paths, gcnew GitCommitArgs(), commitId);
}

bool GitClient::Commit(System::Collections::Generic::ICollection<String^> ^paths, GitCommitArgs ^args, [Out] GitId ^%commitId)
{
	if (! paths)
		throw gcnew ArgumentNullException("paths");
	else if (paths->Count == 0)
		throw gcnew ArgumentOutOfRangeException("Empty list", "paths");

	GitRepository repo;

	array<String^>^ pathArray = dynamic_cast<array<String^>^>(paths);

	if (!pathArray)
	{
		pathArray = gcnew array<String^>(paths->Count);
		paths->CopyTo(pathArray, 0);
	}

	if (!repo.Locate(pathArray[0], args))
		return false;

	// TODO: Map paths

	GitId^ id;
	if (!repo.Index->CreateTree(args, id))
		return false;

	GitTree^ tree;

	if (!repo.Lookup(id, args, tree))
		return false;

	array<GitCommit^>^ parents = nullptr;
	GitReference^ ref = repo.Head;
	if (ref)
	{
		GitId ^commitId;
		if (repo.ResolveReference(ref, commitId))
		{
			parents = gcnew array<GitCommit^>(1);
			if (! repo.GetCommit(commitId, parents[0]))
				parents = nullptr;
		}
	}

	return repo.Commit(tree, safe_cast<ICollection<GitCommit^>^>(parents), args, commitId);
}
