#include "stdafx.h"

#include "GitRepository.h"
#include "GitReference.h"

using namespace SharpGit;
using namespace SharpGit::Implementation;
using namespace SharpGit::Plumbing;
using System::Collections::Generic::List;

struct git_reference {};

bool GitReference::IsDisposed::get()
{
	if (_repository->IsDisposed)
		return true;

	return ! (_reference || _name);
}

String^ GitReference::Name::get()
{
	if (!_name && _reference && !_repository->IsDisposed)
	{
		_name = GitBase::Utf8_PtrToString(git_reference_name(_reference));
	}

	return _name;
}

git_reference* GitReference::Handle::get()
{
	if (IsDisposed)
		throw gcnew ObjectDisposedException("reference");

	// Not thread safe!
	if (!_reference && _name)
	{
		GitPool pool(_repository->Pool);
		git_reference *reference;
		int r = git_reference_lookup(&reference, _repository->Handle, pool.AllocString(Name));

		if (r == 0)
			_reference = reference;
	}

	return _reference;
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

GitReferenceCollection^ GitRepository::References::get()
{
	if (! _references)
		gcnew GitReferenceCollection(this);

	return _references;
}

System::Collections::Generic::IEnumerator<GitReference^>^ GitReferenceCollection::GetEnumerator()
{
	return GetEnumerable()->GetEnumerator();
}

private ref class RefWalkInfo
{
public:
	List<GitReference^>^ references;
	GitRepository^ repository;
};

static int __cdecl for_reference(git_reference *reference, void *payload)
{
	GitRoot<RefWalkInfo^> root(payload);

	root->references->Add(gcnew GitReference(root->repository, reference));
	return 0;
}

System::Collections::Generic::IEnumerable<GitReference^>^ GitReferenceCollection::GetEnumerable()
{
	List<GitReference^> ^references = gcnew List<GitReference^>();
	if (_repository->IsDisposed)
		return references->AsReadOnly();

	GitRoot<RefWalkInfo^> root(gcnew RefWalkInfo());
	root->references = references;
	root->repository = _repository;

	int r = git_reference_foreach(_repository->Handle, for_reference, root.GetBatonValue());
	if (r != 0)
		references->Clear();

	return references->AsReadOnly();
}
