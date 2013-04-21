#pragma once
#include "GitClientContext.h"

namespace SharpGit {
	namespace Plumbing {
		ref class GitRepository;

		public ref class GitCommit : public Implementation::GitBase
		{
		private:
			git_commit *_commit;
			initonly GitRepository ^_repository;

			!GitCommit()
			{
				if (_commit)
				{
					try
					{
						git_commit_free(_commit);
					}
					finally
					{
						_commit = nullptr;
					}
				}
			}

			~GitCommit()
			{
				try
					{
						git_commit_free(_commit);
					}
					finally
					{
						_commit = nullptr;
					}
			}

		internal:
			GitCommit(GitRepository^ repository, git_commit *handle)
			{
				if (! repository)
					throw gcnew ArgumentNullException("repository");
				else if (! handle)
					throw gcnew ArgumentNullException("handle");

				_repository = repository;
				_commit = handle;
			}
				

			property git_commit* Handle
			{
				git_commit* get()
				{
					if (IsDisposed)
						throw gcnew InvalidOperationException();
					return _commit;
				}
			}

		public:
			property bool IsDisposed
			{
				bool get();
			}

			ref class CommitParentCollection sealed : public ICollection<GitCommit^>
			{
				initonly GitCommit ^_commit;

			internal:
				CommitParentCollection(GitCommit ^commit)
				{
					if (! commit)
						throw gcnew ArgumentNullException("commit");

					_commit = commit;
				}

			public:
				property int Count
				{
					virtual int get()
					{
						if (_commit->IsDisposed)
							return 0;

						return git_commit_parentcount(_commit->Handle);
					}
				}

				property GitCommit^ default[int]
				{
					GitCommit^ get(int index)
					{
						git_commit *commit;

						if (index < 0)
							throw gcnew ArgumentOutOfRangeException("index");

						int r = git_commit_parent(&commit, _commit->Handle, index);
						if (!r)
							return gcnew GitCommit(_commit->_repository, commit);

						return nullptr;
					}
				}

				virtual IEnumerator<GitCommit^>^ GetEnumerator()
				{
					array<GitCommit^>^ items = gcnew array<GitCommit^>(Count);

					int c = git_commit_parentcount(_commit->Handle);

					for(int i = 0; i < c; i++)
					{
						git_commit *commit;
						int r = git_commit_parent(&commit, _commit->Handle, i);
						if (!r)
							items[i] = gcnew GitCommit(_commit->_repository, commit);
					}

					return safe_cast<IEnumerator<GitCommit^>^>(items->GetEnumerator());
				}

				property bool IsReadOnly
				{
					virtual bool get()
					{
						return true;
					}
				}

				virtual bool Contains(GitCommit^ commit)
				{
					for each(GitCommit^ c in this)
					{
						if (c->Equals(commit))
							return true;
					}
					return false;
				}

			private:
				virtual System::Collections::IEnumerator^ GetObjectEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator
				{
					return GetEnumerator();
				}

				virtual void Add(GitCommit^ item) sealed = ICollection<GitCommit^>::Add
				{
					UNUSED(item);
					throw gcnew InvalidOperationException();
				}

				virtual bool Remove(GitCommit^ item) sealed = ICollection<GitCommit^>::Remove
				{
					UNUSED(item);
					throw gcnew InvalidOperationException();
				}

				virtual void Clear() sealed = ICollection<GitCommit^>::Clear
				{
					throw gcnew InvalidOperationException();
				}

				virtual void CopyTo(array<GitCommit^>^ toArray, int index) sealed = ICollection<GitCommit^>::CopyTo
				{
					for each(GitCommit^ c in this)
					{
						toArray[index++] = c;
					}
				}
			};

		private:
			CommitParentCollection ^_parents;
		public:
			property CommitParentCollection^ Parents
			{
				CommitParentCollection^ get()
				{
					if (!_parents)
						_parents = gcnew CommitParentCollection(this);

					return _parents;
				}
			}

		};
	}
}
