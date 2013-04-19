#pragma once
#include "GitClientContext.h"
#include "../GitClient/GitClientEventArgs.h"

namespace SharpGit {
	ref class GitStatusArgs;

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

		public ref class GitRepository : public Implementation::GitBase
		{
		private:
			git_repository *_repository;

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
			property bool IsEmpty
			{
				bool get();
			}

			property bool IsBare
			{
				bool get();
			}

			property String^ RepositoryPath
			{
				String^ get();
			}

			property String^ WorkingPath
			{
				String^ get();
				void set(String ^value);
			}

		private:
			GitIndex ^_indexRef;
			GitObjectDatabase ^_dbRef;
			void ClearReferences()
			{
				_indexRef = nullptr;
				_dbRef = nullptr;
			}

		public:
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

		public:
			// Cache and provide as property?
			GitConfiguration^ GetConfigurationInstance();
			void SetConfiguration(GitConfiguration ^newConfig);

			// Cache and provide as property?
			GitIndex^ GetIndexInstance();
			void SetIndex(GitIndex ^newIndex);

			// Cache and provide as property?
			GitObjectDatabase^ GetObjectDatabaseInstance();
			void SetObjectDatabase(GitObjectDatabase ^newDatabase);

		public:
			bool Status(String ^path, GitStatusArgs ^args, EventHandler<GitStatusEventArgs^>^ handler);
		};


	};
}
