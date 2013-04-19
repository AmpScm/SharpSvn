#include "stdafx.h"

#include "GitRepository.h"

#include "GitConfiguration.h"
#include "GitIndex.h"
#include "GitObjectDatabase.h"

using namespace SharpGit;
using namespace SharpGit::Plumbing;
using namespace SharpGit::Implementation;

struct git_repository {};

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
	
	int r = git_repository_set_workdir(_repository, pool.AllocDirent(value));
	if (r)
	{
		(gcnew GitNoArgs())->HandleGitError(this, r);
		throw gcnew InvalidOperationException();
	}
}

GitConfiguration^ GitRepository::GetConfigurationInstance()
{
	AssertOpen();

	git_config *config;
	int r = git_repository_config(&config, _repository);
	if (r)
	{
		(gcnew GitNoArgs())->HandleGitError(this, r);
		throw gcnew InvalidOperationException();
	}

	return gcnew GitConfiguration(config);
}

void GitRepository::SetConfiguration(GitConfiguration^ newConfig)
{
	if (! newConfig)
		throw gcnew ArgumentNullException("newConfig");

	AssertOpen();

	git_repository_set_config(_repository, newConfig->Handle);
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

	git_repository_set_odb(_repository, newDatabase->Handle);
}
