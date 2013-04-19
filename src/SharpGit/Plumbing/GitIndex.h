#pragma once
#include "GitClientContext.h"

namespace SharpGit {
	ref class GitAddArgs;

	namespace Implementation {
		ref class GitPool;
	}

	namespace Plumbing {
		using ::SharpGit::Implementation::GitPool;

		ref class GitObjectDatabase;

		public ref class GitIndex : public Implementation::GitBase
		{
		private:
			git_index *_index;

			!GitIndex()
			{
				if (_index)
				{
					try
					{
						git_index_free(_index);
					}
					finally
					{
						_index = nullptr;
					}
				}
			}

			~GitIndex()
			{
				try
					{
						git_index_free(_index);
					}
					finally
					{
						_index = nullptr;
					}
			}

		internal:
			GitIndex(git_index *index)
			{
				if (! index)
					throw gcnew ArgumentNullException("index");

				_index = index;
			}

			property git_index* Handle
			{
				git_index* get()
				{
					if (!_index)
						throw gcnew InvalidOperationException();
					return _index;
				}
			}

		private:
			void AssertOpen();

		public:
			bool Add(String ^relPath);
			bool Add(String ^relPath, GitAddArgs ^args);
		internal:
			bool Add(const char *relPath, GitAddArgs ^args, GitPool ^pool);

		public:
			property int Count
			{
				int get()
				{
					if (!_index)
						return -1;
					else
						return git_index_entrycount(_index);
				}
			}

			property int UnmergedCount
			{
				int get()
				{
					if (!_index)
						return -1;
					else
						return git_index_entrycount_unmerged(_index);
				}
			}

			bool Contains(String ^relPath);

		public:
			bool Write();
			bool Write(GitArgs ^args);
		};
	}
}