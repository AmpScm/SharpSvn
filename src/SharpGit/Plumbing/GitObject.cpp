#include "stdafx.h"

#include "GitRepository.h"
#include "GitObject.h"
#include "GitBlob.h"
#include "GitCommit.h"
#include "GitTag.h"
#include "GitTree.h"

#include "../GitClient/GitCommitCmd.h"

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

generic<typename T> where T : GitObject
bool GitRepository::Lookup(GitId ^id, [Out] T% object)
{
	return Lookup(id, gcnew GitNoArgs(), object);
}

generic<typename T> where T : GitObject
bool GitRepository::Lookup(GitId ^id, GitArgs ^args, [Out] T% object)
{
	if (! id)
		throw gcnew ArgumentNullException("id");
	else if (! args)
		throw gcnew ArgumentNullException("args");

	AssertOpen();

	GitObject ^ob;
	object = (T)(Object^)nullptr;

	if (Lookup(id, GitObject::ObjectKind<T>(), args, ob))
	{
		object = safe_cast<T>(ob);
		return (Object^)object != nullptr;
	}

	return false;
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

bool GitRepository::LookupViaPrefix(String ^idPrefix, [Out] GitId^% id)
{
	GitObject^ ob;
	id = nullptr;

	if (LookupViaPrefix(idPrefix, ob))
	{
		id = ob->Id;
		delete ob;
		return true;
	}

	return false;
}

bool GitRepository::LookupViaPrefix(String ^idPrefix, [Out] GitObject^% object)
{
	if (String::IsNullOrEmpty(idPrefix))
		throw gcnew ArgumentNullException("idPrefix");

	AssertOpen();

	object = nullptr;

	GitId ^id = GitId::FromPrefix(idPrefix);

	git_oid oid = id->AsOid();
	git_object *obj;
	int r = git_object_lookup_prefix(&obj, Handle, &oid, idPrefix->Length, GIT_OBJ_ANY);

	if (!r && obj)
	{
		object = GitObject::Create(this, obj);
		return true;
	}

	return false;
}


bool LookupViaPrefix(String ^idPrefix, [Out] GitObject^% object);

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
		throw gcnew ObjectDisposedException("object");

	GitObjectKind kind = ObjectKind<T>();

	git_object *peeled;
	int r = git_object_peel(&peeled, Handle, (git_otype)kind);

	if (!r && peeled)
		return safe_cast<T>(Create(Repository, peeled));

	return safe_cast<T>((Object^)nullptr);
}

