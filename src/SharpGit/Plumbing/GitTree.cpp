#include "stdafx.h"

#include "GitTree.h"

using namespace System;
using namespace SharpGit;
using namespace SharpGit::Implementation;
using namespace SharpGit::Plumbing;

struct git_tree_entry {};

GitTreeEntry^ GitTree::default::get(int index)
{
	if (index < 0)
		throw gcnew ArgumentOutOfRangeException("index");

	const git_tree_entry *entry = git_tree_entry_byindex(Handle, index);

	if (! entry)
		throw gcnew ArgumentOutOfRangeException("index");

	return gcnew GitTreeEntry(this, entry);
}

GitTreeEntry^ GitTree::default::get(String ^relPath)
{
	if (String::IsNullOrEmpty(relPath))
		throw gcnew ArgumentOutOfRangeException("relPath");

	GitPool pool;

	const git_tree_entry *entry = git_tree_entry_byname(Handle, pool.AllocRelpath(relPath));

	if (! entry)
		throw gcnew ArgumentOutOfRangeException("index");

	return gcnew GitTreeEntry(this, entry);
}

bool GitTree::Contains(String ^relPath)
{
	if (String::IsNullOrEmpty(relPath))
		throw gcnew ArgumentOutOfRangeException("relPath");

	GitPool pool;

	const git_tree_entry *entry = git_tree_entry_byname(Handle, pool.AllocRelpath(relPath));

	return (entry != nullptr);
}

IEnumerator<GitTreeEntry^>^ GitTree::GetEnumerator()
{
	int count = git_tree_entrycount(Handle);
	array<GitTreeEntry^> ^items = gcnew array<GitTreeEntry^>(count);

	for (int i = 0; i < count; i++)
	{
		items[i] = gcnew GitTreeEntry(this, git_tree_entry_byindex(_tree, i));
	}

	ICollection<GitTreeEntry^>^ col = safe_cast<ICollection<GitTreeEntry^>^>(items);
	return col->GetEnumerator();
}

ICollection<GitTreeEntry^>^ GitTreeEntry::Children::get()
{
	if (!_children && !_tree->IsDisposed)
	{
		if (Kind == GitObjectKind::Tree)
		{
			const git_oid* id = git_tree_entry_id(_entry);
			git_repository *owner = git_tree_owner(_tree->Handle);
			git_tree *tree;
			
			int r = git_tree_lookup(&tree, owner, id);

			if (!r)
				_children = gcnew GitTree(tree);
		}

		if ((Object^)_children == nullptr)
			_children = safe_cast<ICollection<GitTreeEntry^>^>(gcnew array<GitTreeEntry^>(0));
	}

	return _children;
}