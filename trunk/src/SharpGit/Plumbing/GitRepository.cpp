#include "stdafx.h"

#include "GitRepository.h"
#include "../GitClient/GitStatus.h"
#include "../GitClient/GitCommitCmd.h"

#include "GitConfiguration.h"
#include "GitIndex.h"
#include "GitObjectDatabase.h"
#include "GitTree.h"
#include "GitCommit.h"
#include "GitReference.h"

using namespace System;
using namespace SharpGit;
using namespace SharpGit::Plumbing;
using namespace SharpGit::Implementation;

struct git_repository {};

void GitRepository::ClearReferences()
{
	try
	{
		if (_configuration)
			delete _configuration;
		if (_indexRef)
			delete _indexRef;
		if (_dbRef)
			delete _dbRef;
	}
	finally
	{
		_configuration = nullptr;
		_indexRef = nullptr;
		_dbRef = nullptr;
	}
}

bool GitRepository::Open(String ^repositoryPath)
{
	return Open(repositoryPath, gcnew GitNoArgs());
}

bool GitRepository::Open(String ^repositoryPath, GitArgs ^args)
{
	AssertNotOpen();

	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (! args)
		throw gcnew ArgumentNullException("args");

	GitPool pool;

	git_repository *repository;
	int r = git_repository_open(&repository, pool.AllocDirent(repositoryPath));

	if (r)
		return args->HandleGitError(this, r);

	_repository = repository;
	return true;
}

bool GitRepository::Locate(String ^path)
{
	return Locate(path, gcnew GitNoArgs());
}

bool GitRepository::Locate(String ^path, GitArgs ^args)
{
	AssertNotOpen();

	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (! args)
		throw gcnew ArgumentNullException("args");

	GitPool pool;

	const char *ceiling_dirs = nullptr;

	git_repository *repository;
	int r = git_repository_open_ext(&repository,
									pool.AllocDirent(path),
									GIT_REPOSITORY_OPEN_CROSS_FS,
									ceiling_dirs);

	if (r)
		return args->HandleGitError(this, r);

	_repository = repository;
	return true;
}

GitRepository^ GitRepository::Create(String ^repositoryPath)
{
	return Create(repositoryPath, gcnew GitRepositoryCreateArgs());
}

GitRepository^ GitRepository::Create(String ^repositoryPath, GitRepositoryCreateArgs ^args)
{
	GitRepository^ repo = gcnew GitRepository();
	bool ok = false;
	try
	{
		if (repo->CreateAndOpen(repositoryPath, args))
		{
			ok = true;
			return repo;
		}

		throw gcnew InvalidOperationException();
	}
	finally
	{
		if (!ok)
			delete repo;
	}
}

bool GitRepository::CreateAndOpen(String ^repositoryPath)
{
	AssertNotOpen();
	return CreateAndOpen(repositoryPath, gcnew GitRepositoryCreateArgs());
}

bool GitRepository::CreateAndOpen(String ^repositoryPath, GitRepositoryCreateArgs ^args)
{
	AssertNotOpen();

	GitPool pool;
	git_repository *repository;
	int r = git_repository_init(&repository, pool.AllocDirent(repositoryPath), args->CreateBareRepository);

	if (r)
		return args->HandleGitError(this, r);

	_repository = repository;
	return true;
}

bool GitRepository::IsEmpty::get()
{
	AssertOpen();

	return git_repository_is_empty(_repository) != 0;
}

bool GitRepository::IsBare::get()
{
	AssertOpen();

	return git_repository_is_bare(_repository) != 0;
}

String^ GitRepository::RepositoryPath::get()
{
	AssertOpen();

	const char *path = git_repository_path(_repository);

	return GitBase::StringFromDirentNoPool(path);
}

String^ GitRepository::WorkingPath::get()
{
	AssertOpen();

	const char *path = git_repository_workdir(_repository);

	return GitBase::StringFromDirentNoPool(path);
}

void GitRepository::WorkingPath::set(String ^value)
{
	AssertOpen();

	if (String::IsNullOrEmpty(value))
		throw gcnew ArgumentNullException("value");

	GitPool pool;
	
	int r = git_repository_set_workdir(_repository, pool.AllocDirent(value), FALSE);
	if (r)
	{
		(gcnew GitNoArgs())->HandleGitError(this, r);
		throw gcnew InvalidOperationException();
	}
}

GitConfiguration^ GitRepository::Configuration::get()
{
	if (IsDisposed)
		return nullptr;

	AssertOpen();

	git_config *config;
	int r = git_repository_config(&config, _repository);
	if (r)
	{
		(gcnew GitNoArgs())->HandleGitError(this, r);
		throw gcnew InvalidOperationException();
	}

	return _configuration = gcnew GitConfiguration(this, config);
}

GitIndex^ GitRepository::GetIndexInstance()
{
	AssertOpen();

	git_index *index;
	int r = git_repository_index(&index, _repository);
	if (r)
	{
		(gcnew GitNoArgs())->HandleGitError(this, r);
		throw gcnew InvalidOperationException();
	}

	return gcnew GitIndex(index);
}

void GitRepository::SetIndex(GitIndex ^newIndex)
{
	if (! newIndex)
		throw gcnew ArgumentNullException("newIndex");

	AssertOpen();

	if (Object::ReferenceEquals(newIndex, _indexRef))
		return;

	ClearReferences();
	git_repository_set_index(_repository, newIndex->Handle);
}

// Cache and provide as property?
GitObjectDatabase^ GitRepository::GetObjectDatabaseInstance()
{
	AssertOpen();

	git_odb *odb;
	int r = git_repository_odb(&odb, _repository);
	if (r)
	{
		(gcnew GitNoArgs())->HandleGitError(this, r);
		throw gcnew InvalidOperationException();
	}

	return gcnew GitObjectDatabase(odb);
}

void GitRepository::SetObjectDatabase(GitObjectDatabase ^newDatabase)
{
	if (! newDatabase)
		throw gcnew ArgumentNullException("newDatabase");

	AssertOpen();

	if (Object::ReferenceEquals(newDatabase, _dbRef))
		return;

	ClearReferences();
	git_repository_set_odb(_repository, newDatabase->Handle);
}

const char *GitRepository::MakeRelpath(String ^path, GitPool ^pool)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (! pool)
		throw gcnew ArgumentNullException("pool");

	AssertOpen();

	const char *wrk_path = svn_dirent_canonicalize(
								git_repository_workdir(_repository),
								pool->Handle);
	const char *item_path = pool->AllocDirent(path);

	return svn_dirent_skip_ancestor(wrk_path, item_path);
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
		tree = gcnew GitTree(the_tree);
	else
		tree = nullptr;
	
	return args->HandleGitError(this, r);
}

GitReference^ GitRepository::Head::get()
{
	if (IsDisposed)
		return nullptr;

	git_reference *ref;
	int r = git_repository_head(&ref, _repository);

	if (!r)
		return gcnew GitReference(this, ref);

	return nullptr;
}

bool GitRepository::ResolveReference(String^ referenceName, [Out] GitId ^%id)
{
	if (String::IsNullOrEmpty(referenceName))
		throw gcnew ArgumentNullException("referenceName");

	AssertOpen();

	GitPool pool;
	git_oid oid;
	int r = git_reference_name_to_id(&oid, _repository, pool.AllocString(referenceName));

	if (r == 0)
	{
		id = gcnew GitId(oid);
		return true;
	}
	else
	{
		id = nullptr;
		return false;
	}
}

bool GitRepository::ResolveReference(GitReference^ reference, [Out] GitId ^%id)
{
	if (! reference)
		throw gcnew ArgumentNullException("reference");

	AssertOpen();

	GitPool pool;
	git_reference *ref;
	int r = git_reference_resolve(&ref, reference->Handle);

	if (r == 0)
	{
		const git_oid *oid = git_reference_target(ref);
		id = gcnew GitId(oid);

		git_reference_free(ref);

		return true;
	}
	else
	{
		id = nullptr;
		return false;
	}
}

bool GitRepository::GetCommit(GitId ^id, [Out] GitCommit ^%commit)
{
	if (! id)
		throw gcnew ArgumentNullException("id");

	AssertOpen();

	git_commit *cmt;
	git_oid oid = id->AsOid();
	int r = git_commit_lookup(&cmt, _repository, &oid);

	if (r == 0)
	{
		commit = gcnew GitCommit(this, cmt);
		return true;
	}
	else
	{
		commit = nullptr;
		return false;
	}
}

bool GitRepository::Commit(GitTree ^tree, ICollection<GitCommit^>^ parents, GitCommitArgs ^args)
{
	GitId ^ignored;

	return Commit(tree, parents, args, ignored);
}

bool GitRepository::Commit(GitTree ^tree, ICollection<GitCommit^>^ parents, GitCommitArgs ^args, [Out] GitId^% id)
{
	if (!tree)
		throw gcnew ArgumentNullException("tree");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	AssertOpen();

	GitPool pool;

	const git_commit** commits = nullptr;
	int nCommits = 0;

	if (parents)
	{
		// Theoretical problem: External change of parents might change count
		commits = (const git_commit**)alloca(sizeof(git_commit*) * parents->Count);

		for each (GitCommit^ c in parents)
			commits[nCommits++] = c->Handle;
	}

	bool normalize = args->NormalizeLogMessage;
	bool strip = args->StripLogMessageComments;
	const char *msg;
	if (normalize || strip)
	{
		String^ msgString = args->LogMessage ? args->LogMessage : "";
		msgString = msgString->Replace("\r", "");
		msg = pool.AllocString(msgString);
		size_t sz = strlen(msg);
		sz += sz/4 + 4;
		char *result = (char*)pool.Alloc(sz+1);

		int r = git_message_prettify(result, sz, msg, strip);

		if (r < 0)
		{
			int len = git_message_prettify(NULL, 0, msg, strip);

			if (len >= 0)
			{
				result = (char*)pool.Alloc(sz+1);

				r = git_message_prettify(result, sz, msg, strip);
			}
		}

		if (r < 0)
			return args->HandleGitError(this, r);

		msg = result;
	}
	else
		msg = args->LogMessage ? pool.AllocString(args->LogMessage) : "";

	git_oid commit_id;
	int r = git_commit_create(&commit_id, _repository,
							  pool.AllocString(args->UpdateReference),
							  args->Author->Alloc(%pool),
							  args->Committer->Alloc(%pool),
							  NULL /* utf-8 */,
							  msg,
							  tree->Handle,
							  nCommits, commits);

	if (! r)
		id = gcnew GitId(commit_id);
	else
		id = nullptr;
	
	return args->HandleGitError(this, r);
}

String ^GitRepository::MakeRelativePath(String ^path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	GitPool pool(_pool);

	return Utf8_PtrToString(MakeRelpath(path, %pool));
}

#pragma region STATUS

static int __cdecl on_status(const char *path, unsigned int status, void *baton)
{
	GitRoot<GitStatusArgs^> args(baton);
	GitPool pool(args.GetPool());
	try
	{
		GitStatusEventArgs^ ee = gcnew GitStatusEventArgs(path, args.GetWcPath(), status, args, %pool);

		args->OnStatus(ee);
	}
	catch(Exception^ e)
	{
		return args->WrapException(e);
	}

	return 0;
}

bool GitRepository::Status(String ^path, GitStatusArgs ^args, EventHandler<GitStatusEventArgs^>^ handler)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	if (handler)
		args->Status += handler;

	try
	{
		GitPool pool;
		const char *wcPath = svn_dirent_canonicalize(git_repository_workdir(_repository), pool.Handle);

		GitRoot<GitStatusArgs^> root(args, wcPath, %pool);

		int r = git_status_foreach_ext(_repository,
									   args->MakeOptions(path, %pool),
									   on_status,
									   root.GetBatonValue());

		return args->HandleGitError(this, r);
	}
	finally
	{
		if (handler)
			args->Status -= handler;
	}
}

#pragma endregion STATUS
