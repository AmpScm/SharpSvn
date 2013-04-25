#pragma once
#include "GitClientContext.h"
#include "../GitClient/GitClientEventArgs.h"

namespace SharpGit {
	ref class GitStatusArgs;
	ref class GitCommitArgs;

	namespace Plumbing {

		public ref class GitRepositoryCreateArgs : GitArgs
		{
			bool _bare;
		public:
			property bool CreateBareRepository
			{
				bool get()
				{
					return _bare;
				}
				void set(bool value)
				{
					_bare = value;
				}
			}
		};

		ref class GitConfiguration;
		ref class GitIndex;
		ref class GitObjectDatabase;
		ref class GitTree;
		ref class GitCommit;
		ref class GitReference;

		public enum class GitRepositoryState
		{
			None = 0, // GIT_REPOSITORY_STATE_NONE
			Merge = GIT_REPOSITORY_STATE_MERGE,
			Revert = GIT_REPOSITORY_STATE_REVERT,
			CherryPick = GIT_REPOSITORY_STATE_CHERRY_PICK,
			Bisect = GIT_REPOSITORY_STATE_BISECT,
			Rebase = GIT_REPOSITORY_STATE_REBASE,
			RebaseInteractive = GIT_REPOSITORY_STATE_REBASE_INTERACTIVE,
			RebaseMerge = GIT_REPOSITORY_STATE_REBASE_MERGE,
			MailBox = GIT_REPOSITORY_STATE_APPLY_MAILBOX,
			MailBoxOrRebase = GIT_REPOSITORY_STATE_APPLY_MAILBOX_OR_REBASE
		};

		public ref class GitRepository : public Implementation::GitBase
		{
		private:
			git_repository *_repository;
			GitPool _pool;

			GitRepository(git_repository *repository)
			{
				if (! repository)
					throw gcnew ArgumentNullException("repository");

				_repository = repository;
			}

			!GitRepository()
			{
				if (_repository)
				{
					try
					{
						ClearReferences();
						git_repository_free(_repository);
					}
					finally
					{
						_repository = nullptr;
					}
				}
			}

			~GitRepository()
			{
				try
				{
					ClearReferences();
					git_repository_free(_repository);
				}
				finally
				{
					_repository = nullptr;
				}
			}

		internal:
			bool Locate(String ^path, GitArgs ^args);
			bool Open(String ^repositoryPath, GitArgs ^args);

			void AssertNotOpen()
			{
				if (_repository)
					throw gcnew InvalidOperationException();
			}

			void AssertOpen()
			{
				if (! _repository)
					throw gcnew InvalidOperationException();
			}

			property git_repository* Handle
			{
				git_repository* get()
				{
					if (IsDisposed)
						throw gcnew InvalidOperationException();

					return _repository;
				}
			}

		public:
			property bool IsDisposed
			{
				bool get()
				{
					return !_repository;
				}
			}

		public:
			/// <summary>Creates an unopened repository</summary>
			GitRepository() {}

			/// <summary>Creates an unopened repository and then calls Open(REPOSITORYPATH)</summary>
			GitRepository(String^ repositoryPath)
			{
				if (!Open(repositoryPath))
					throw gcnew InvalidOperationException();
			}

		public:
			/// <summary>Opens the repository containing PATH</summary>
			bool Locate(String ^path);

			/// <summary>Opens the repository at REPOSITORYPATH</summary>
			bool Open(String ^repositoryPath);

			bool CreateAndOpen(String ^repositoryPath);
			bool CreateAndOpen(String ^repositoryPath, GitRepositoryCreateArgs ^args);

			static GitRepository^ Create(String ^repositoryPath);
			static GitRepository^ Create(String ^repositoryPath, GitRepositoryCreateArgs ^args);

		public:
			bool Lookup(GitId ^id, [Out] GitTree^% tree);
			bool Lookup(GitId ^id, GitArgs ^args, [Out] GitTree^% tree);

			bool Commit(GitTree ^tree, ICollection<GitCommit^> ^parents, GitCommitArgs ^args);
			bool Commit(GitTree ^tree, ICollection<GitCommit^> ^parents, GitCommitArgs ^args, [Out] GitId^% id);

		public:
			property bool IsEmpty
			{
				bool get();
			}

			property bool IsBare
			{
				bool get();
			}

			property String^ RepositoryDirectory
			{
				String^ get();
			}

			property String^ WorkingCopyDirectory
			{
				String^ get();
				void set(String ^value);
			}

		public:
			property GitReference^ Head
			{
				GitReference^ get();
			}

			property bool IsHeadDetached
			{
				bool get()
				{
					if (IsDisposed)
						return false;

					return (git_repository_head_detached(_repository) == 1);
				}
			}

			property bool IsHeadOrphaned
			{
				bool get()
				{
					if (IsDisposed)
						return false;

					return (git_repository_head_orphan(_repository) == 1);
				}
			}

		public:
			bool ResolveReference(String^ referenceName, [Out] GitId ^%id);
			bool ResolveReference(GitReference^ reference, [Out] GitId ^%id);
			bool GetCommit(GitId ^id, [Out] GitCommit ^%commit);

		internal:
			const char *MakeRelpath(String ^path, GitPool ^pool);

		public:
			String ^MakeRelativePath(String ^path);

		private:
			GitConfiguration ^_configuration;
			GitIndex ^_indexRef;
			GitObjectDatabase ^_dbRef;
			void ClearReferences();

		public:
			property GitConfiguration^ Configuration
			{
				GitConfiguration^ get();
			}

			property GitIndex^ Index
			{
				GitIndex^ get()
				{
					if (!_indexRef && _repository)
						_indexRef = GetIndexInstance();

					return _indexRef;
				}
			}

			property GitObjectDatabase^ ObjectDatabase
			{
				GitObjectDatabase^ get()
				{
					if (!_dbRef && _repository)
						_dbRef = GetObjectDatabaseInstance();

					return _dbRef;
				}
			}

			property GitRepositoryState RepositoryState
			{
				GitRepositoryState get()
				{
					if (IsDisposed)
						return GitRepositoryState::None;

					return (GitRepositoryState)git_repository_state(_repository);
				}
			}

		public:
			bool OpenTree(GitId^ id, [Out] GitTree ^%tree);
			bool OpenTree(GitId^ id, GitArgs^ args, [Out] GitTree ^%tree);

		public:
			void SetIndex(GitIndex ^newIndex);
			void SetObjectDatabase(GitObjectDatabase ^newDatabase);

		private:
			GitIndex^ GetIndexInstance();
			GitObjectDatabase^ GetObjectDatabaseInstance();

		public:
			bool Status(String ^path, GitStatusArgs ^args, EventHandler<GitStatusEventArgs^>^ handler);
		};


	};
}
