#include "stdafx.h"

#include "GitIndex.h"
#include "GitTree.h"
#include "GitClient/GitAdd.h"

using namespace System;
using namespace SharpGit;
using namespace SharpGit::Implementation;
using namespace SharpGit::Plumbing;

struct git_index {};

void GitIndex::AssertOpen()
{
	if (! _index)
		throw gcnew InvalidOperationException();
}

bool GitIndex::Add(String ^relPath)
{
	AssertOpen();

	return Add(relPath, gcnew GitAddArgs());
}

bool GitIndex::Add(String ^relPath, GitAddArgs ^args)
{
	AssertOpen();

	GitPool pool;

	return Add(pool.AllocRelpath(relPath), args, %pool);
}

bool GitIndex::Add(const char *relPath, GitAddArgs ^args, GitPool ^pool)
{
	UNUSED(pool);
	if (! relPath || !*relPath)
		throw gcnew ArgumentNullException("relPath");
	else if (! args)
		throw gcnew ArgumentNullException("args");
	AssertOpen();

	int r;

	// TODO: Use append2/index2 for advanced options
	
	if (args->Append)
		r = git_index_append(_index, relPath, args->Stage);
	else
		r = git_index_add(_index, relPath, args->Stage);

	return args->HandleGitError(this, r);
}

bool GitIndex::Contains(String ^relPath)
{
	AssertOpen();

	GitPool pool;

	return git_index_find(_index, pool.AllocRelpath(relPath)) >= 0;
}

bool GitIndex::Write()
{
	return Write(gcnew GitNoArgs());
}

bool GitIndex::Write(GitArgs ^args)
{
	if (! args)
		throw gcnew ArgumentNullException("args");

	AssertOpen();

	int r = git_index_write(_index);

	return args->HandleGitError(this, r);
}

bool GitIndex::Reload()
{
	return Reload(gcnew GitNoArgs());
}

bool GitIndex::Reload(GitArgs ^args)
{
	if (! args)
		throw gcnew ArgumentNullException("args");

	AssertOpen();

	int r = git_index_read(_index);

	return args->HandleGitError(this, r);
}

bool GitIndex::Normalize()
{
	AssertOpen();

	git_index_uniq(_index);
	return true;
}


bool GitIndex::Clear()
{
	AssertOpen();

	git_index_clear(_index);
	return true;
}

bool GitIndex::Read(GitTree ^replacementTree)
{
	return Read(replacementTree, gcnew GitNoArgs());
}

bool GitIndex::Read(GitTree ^replacementTree, GitArgs ^args)
{
	if (! replacementTree)
		throw gcnew ArgumentNullException("replacementTree");
	else if (! args)
		throw gcnew ArgumentNullException("args");

	AssertOpen();

	int r = git_index_read_tree(_index, replacementTree->Handle);

	return args->HandleGitError(this, r);
}

GitIndexEntry^ GitIndex::default::get(int index)
{
	AssertOpen();

	if (index < 0)
		throw gcnew ArgumentOutOfRangeException("index");

	git_index_entry* entry = git_index_get(_index, index);

	if (! entry)
		throw gcnew ArgumentOutOfRangeException("index");

	return gcnew GitIndexEntry(this, entry);
}

GitIndexEntry^ GitIndex::default::get(String^ relativePath)
{
	AssertOpen();

	if (String::IsNullOrEmpty(relativePath))
		throw gcnew ArgumentNullException("relativePath");

	GitPool pool;

	int n = git_index_find(_index, pool.AllocRelpath(relativePath));

	if (n >= 0)
		return default[n];
	else
		throw gcnew ArgumentOutOfRangeException("relativePath");
}

System::Collections::Generic::IEnumerator<GitIndexEntry^>^ GitIndex::GetEnumerator()
{
	AssertOpen();

	int cnt = Count;
	array<GitIndexEntry^>^ items = gcnew array<GitIndexEntry^>(cnt);

	for (int i = 0; i < cnt; i++)
		items[i] = default[i];

	return static_cast<System::Collections::Generic::IEnumerable<GitIndexEntry^>^>(items)->GetEnumerator();
}

bool GitIndex::Remove(int index)
{
	AssertOpen();

	if (index < 0)
		throw gcnew ArgumentOutOfRangeException("index");

	return ! git_index_remove(_index, index);
}

bool GitIndex::Remove(String ^relativePath, bool recursive)
{
	AssertOpen();

	if (String::IsNullOrEmpty(relativePath))
		throw gcnew ArgumentNullException("relativePath");

	GitPool pool;

	const char *path = pool.AllocRelpath(relativePath);

	bool deletedOne = false;
	int c = git_index_entrycount(_index);
	
	for (int i = 0; i < c; i++)
	{
		git_index_entry * entry = git_index_get(_index, i);
		if (recursive ? (svn_relpath_skip_ancestor(path, entry->path) != nullptr)
					  : ! strcmp(path, entry->path))
		{
			git_index_remove(_index, i--);
			deletedOne = true;
		}
	}

	return deletedOne;
}
