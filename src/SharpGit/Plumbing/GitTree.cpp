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

GitTree^ GitTreeEntry::AsTree()
{
	if (!_thisTree && !_tree->IsDisposed && Kind == GitObjectKind::Tree)
	{
		const git_oid* id = git_tree_entry_id(_entry);
		git_repository *owner = git_tree_owner(_tree->Handle);
		git_tree *tree;
			
		int r = git_tree_lookup(&tree, owner, id);

		if (!r)
			_thisTree = gcnew GitTree(tree, this);
	}

	return _thisTree;
}

ICollection<GitTreeEntry^>^ GitTreeEntry::Children::get()
{
	if (!_children && !_tree->IsDisposed)
	{
		if (Kind == GitObjectKind::Tree)
			_children = AsTree();

		if ((Object^)_children == nullptr)
			_children = safe_cast<ICollection<GitTreeEntry^>^>(gcnew array<GitTreeEntry^>(0));
	}

	return _children;
}

IEnumerable<GitTreeEntry^>^ GitTreeEntry::Descendants::get()
{
	if (_thisTree)
		return _thisTree->Descendants;
	else if (!_tree->IsDisposed)
	{
		if (Kind == GitObjectKind::Tree)
			return AsTree()->Descendants;
		else
			return Children; // Easy empty list
	}

	return nullptr;
}


private ref class GitTreeDescendantsWalker sealed : IEnumerator<GitTreeEntry^>, IEnumerable<GitTreeEntry^>
{
private:
	initonly GitTree^ _tree;
	GitTreeDescendantsWalker ^_inner;
	int _index;
	GitTreeEntry ^_current;
	~GitTreeDescendantsWalker()
	{
	}

public:
	GitTreeDescendantsWalker(GitTree ^tree)
	{
		if (! tree)
			throw gcnew ArgumentNullException("tree");

		_tree = tree;
		_index = -1;
	}

	virtual bool MoveNext()
	{
		_current = nullptr;
		if (_inner && _inner->MoveNext())
			return true;

		_inner = nullptr;

		int cnt = _tree->Count;

		if (_index + 1 >= cnt)
			return false;

		_index++;
		_current = _tree[_index];

		if (_current->Kind == GitObjectKind::Tree)
			_inner = gcnew GitTreeDescendantsWalker(_current->AsTree());

		return true;
	}

	virtual property GitTreeEntry^ Current
	{
		GitTreeEntry^ get()
		{
			if (!_current && _inner)
				_current = _inner->Current;

			return _current;
		}
	}

	virtual void Reset()
	{
		_current = nullptr;
		_inner = nullptr;
		_index = -1;
	}

private:
	virtual property Object^ ObjectCurrent
	{
		virtual Object^ get() sealed = System::Collections::IEnumerator::Current::get
		{
			return Current;
		}
	}

	virtual IEnumerator<GitTreeEntry^>^ GetEnumerator() sealed = IEnumerable<GitTreeEntry^>::GetEnumerator
	{
		return gcnew GitTreeDescendantsWalker(_tree);
	}

	virtual System::Collections::IEnumerator^ GetObjectEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator
	{
		return GetEnumerator();
	}
};

IEnumerable<GitTreeEntry^>^ GitTree::Descendants::get()
{
	return gcnew GitTreeDescendantsWalker(this);
}