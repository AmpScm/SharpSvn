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

	// TODO: Use append2/index2 for advanced options
	
	int r = git_index_add_bypath(_index, relPath);

	return args->HandleGitError(this, r);
}

bool GitIndex::Contains(String ^relPath)
{
	AssertOpen();

	GitPool pool;

	size_t pos;
	int r = git_index_find(&pos, _index, pool.AllocRelpath(relPath));

	return !r && (pos >= 0);
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

	const git_index_entry* entry = git_index_get_byindex(_index, index);

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

	const git_index_entry *entry;
	entry = git_index_get_bypath(_index, pool.AllocRelpath(relativePath), 0);

	if (entry)
		return gcnew GitIndexEntry(this, entry);
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

	ICollection<GitIndexEntry^>^ col = safe_cast<ICollection<GitIndexEntry^>^>(items);
	return col->GetEnumerator();
}

bool GitIndex::Remove(int index)
{
	AssertOpen();

	if (index < 0)
		throw gcnew ArgumentOutOfRangeException("index");

	const git_index_entry *entry = git_index_get_byindex(_index, index);

	if (entry)
		return git_index_remove(_index, entry->path, git_index_entry_stage(entry)) == 0;
	else
		return false;
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
		const git_index_entry * entry = git_index_get_byindex(_index, i);
		if (recursive ? (svn_relpath_skip_ancestor(path, entry->path) != nullptr)
					  : ! strcmp(path, entry->path))
		{
			git_index_remove(_index, entry->path, git_index_entry_stage(entry));
			c--;
			deletedOne = true;
		}
	}

	return deletedOne;
}

bool GitIndex::CreateTree([Out] GitId^% id)
{
	return CreateTree(gcnew GitNoArgs(), id);
}

bool GitIndex::CreateTree(GitArgs ^args, [Out] GitId^% id)
{
	if (! args)
		throw gcnew ArgumentNullException("args");

	AssertOpen();

	git_oid oid;
	int r = git_index_write_tree(&oid, _index);

	if (! r)
		id = gcnew GitId(oid);
	else
		id = nullptr;

	return args->HandleGitError(this, r);
}
