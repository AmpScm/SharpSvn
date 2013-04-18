#pragma once
#include "GitClientContext.h"

namespace SharpGit {
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

		};


	};
}
