#include "stdafx.h"

#include "GitRepository.h"
#include "GitObject.h"
#include "GitBlob.h"
#include "GitCommit.h"
#include "GitTag.h"
#include "GitTree.h"


using namespace SharpGit;
using namespace SharpGit::Plumbing;

bool GitObject::IsDisposed::get()
{
	return !_obj && !_repository->IsDisposed;
}

GitId^ GitObject::Id::get()
{
	if (!_id && !IsDisposed)
		_id = gcnew GitId(git_object_id(Handle));

	return _id;
}

GitObject^ GitObject::Create(GitRepository^ repository, git_object *handle)
{
	if (! repository)
		throw gcnew ArgumentNullException("repository");
	else if (! handle)
		throw gcnew ArgumentNullException("handle");

	switch (git_object_type(handle))
	{
	case GIT_OBJ_COMMIT:
		return gcnew GitCommit(repository, reinterpret_cast<git_commit*>(handle));
	case GIT_OBJ_TREE:
		return gcnew GitTree(repository, reinterpret_cast<git_tree*>(handle));
	case GIT_OBJ_BLOB:
		return gcnew GitBlob(repository, reinterpret_cast<git_blob*>(handle));
	case GIT_OBJ_TAG:
		return gcnew GitTag(repository, reinterpret_cast<git_tag*>(handle));

	default:
		return gcnew GitObject(repository, handle);
	}
}

bool GitRepository::Lookup(GitId ^id, [Out] GitTree^% tree)
{
	return Lookup(id, gcnew GitNoArgs(), tree);
}

bool GitRepository::Lookup(GitId ^id, GitArgs ^args, [Out] GitTree^% tree)
{
	if (! id)
		throw gcnew ArgumentNullException("id");

	AssertOpen();

	git_tree *the_tree;
	git_oid tree_id = id->AsOid();
	int r = git_tree_lookup(&the_tree, _repository, &tree_id);

	if (r == 0)
		tree = gcnew GitTree(this, the_tree);
	else
		tree = nullptr;
	
	return args->HandleGitError(this, r);
}

bool GitRepository::Lookup(GitId ^id, [Out] GitObject^% object)
{
	return Lookup(id, gcnew GitNoArgs(), object);
}

bool GitRepository::Lookup(GitId ^id, GitArgs ^args, [Out] GitObject^% object)
{
	if (! id)
		throw gcnew ArgumentNullException("id");
	else if (! args)
		throw gcnew ArgumentNullException("args");

	return Lookup(id, GitObjectKind::Any, args, object);
}

bool GitRepository::Lookup(GitId ^id, GitObjectKind kind, [Out] GitObject^% object)
{
	return Lookup(id, kind, gcnew GitNoArgs(), object);
}

bool GitRepository::Lookup(GitId ^id, GitObjectKind kind, GitArgs ^args, [Out] GitObject^% object)
{
	if (! id)
		throw gcnew ArgumentNullException("id");
	else if (! args)
		throw gcnew ArgumentNullException("args");

	AssertOpen();

	git_object *result;
	git_oid oid = id->AsOid();

	int r = git_object_lookup(&result, Handle, &oid, (git_otype)kind);
	if (!r)
	{
		if (result)
			object = GitObject::Create(this, result);
		else
			object = nullptr;
	}
	return args->HandleGitError(this, r);
}

generic<typename T> where T : GitObject
GitObjectKind GitObject::ObjectKind()
{
	System::Type^ type = T::typeid;

	if (GitTree::typeid == type)
		return GitObjectKind::Tree;
	else if (GitCommit::typeid == type)
		return GitObjectKind::Commit;
	else if (GitBlob::typeid == type)
		return GitObjectKind::Blob;
	else if (GitTag::typeid == type)
		return GitObjectKind::Tag;
	else if (GitObject::typeid == type)
		return GitObjectKind::Any;

	throw gcnew ArgumentOutOfRangeException();
}

generic<typename T> where T : GitObject
T GitObject::Peel()
{
	if (IsDisposed)
		throw gcnew InvalidOperationException();

	GitObjectKind kind = ObjectKind<T>();

	git_object *peeled;
	int r = git_object_peel(&peeled, Handle, (git_otype)kind);

	if (!r && peeled)
		return safe_cast<T>(Create(Repository, peeled));

	return safe_cast<T>((Object^)nullptr);
}
