#pragma once
#include "GitClientContext.h"

namespace SharpGit {
	ref class GitSignature;

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

					ICollection<GitCommit^>^ col = safe_cast<ICollection<GitCommit^>^>(items);
					return col->GetEnumerator();
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

			ref class CommitParentIdCollection sealed : public ICollection<GitId^>
			{
				initonly GitCommit ^_commit;

			internal:
				CommitParentIdCollection(GitCommit ^commit)
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

				property GitId^ default[int]
				{
					GitId^ get(int index)
					{
						if (index < 0)
							throw gcnew ArgumentOutOfRangeException("index");

						const git_oid *oid = git_commit_parent_id(_commit->Handle, index);
						if (oid)
							return gcnew GitId(oid);

						return nullptr;
					}
				}

				virtual IEnumerator<GitId^>^ GetEnumerator()
				{
					array<GitId^>^ items = gcnew array<GitId^>(Count);

					int c = git_commit_parentcount(_commit->Handle);

					for(int i = 0; i < c; i++)
					{
						const git_oid *oid = git_commit_parent_id(_commit->Handle, i);
						if (oid)
							items[i] = gcnew GitId(oid);
					}

					ICollection<GitId^>^ col = safe_cast<ICollection<GitId^>^>(items);

					return col->GetEnumerator();
				}

				property bool IsReadOnly
				{
					virtual bool get()
					{
						return true;
					}
				}

				virtual bool Contains(GitId^ commit)
				{
					for each(GitId^ c in this)
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

				virtual void Add(GitId^ item) sealed = ICollection<GitId^>::Add
				{
					UNUSED(item);
					throw gcnew InvalidOperationException();
				}

				virtual bool Remove(GitId^ item) sealed = ICollection<GitId^>::Remove
				{
					UNUSED(item);
					throw gcnew InvalidOperationException();
				}

				virtual void Clear() sealed = ICollection<GitId^>::Clear
				{
					throw gcnew InvalidOperationException();
				}

				virtual void CopyTo(array<GitId^>^ toArray, int index) sealed = ICollection<GitId^>::CopyTo
				{
					for each(GitId^ c in this)
					{
						toArray[index++] = c;
					}
				}
			};

		private:
			CommitParentCollection ^_parents;
			CommitParentIdCollection ^_parentIds;
			GitCommit^ _ancestor;
			GitSignature ^_author;
			GitSignature ^_committer;
			String ^_logMessage;
			GitId ^_id;

		public:
			property GitId^ Id
			{
				GitId^ get()
				{
					if (!_id && !IsDisposed)
						_id = gcnew GitId(git_commit_id(_commit));

					return _id;
				}
			}

			property CommitParentCollection^ Parents
			{
				CommitParentCollection^ get()
				{
					if (!_parents)
						_parents = gcnew CommitParentCollection(this);

					return _parents;
				}
			}

			property CommitParentIdCollection^ ParentIds
			{
				CommitParentIdCollection^ get()
				{
					if (!_parentIds)
						_parentIds = gcnew CommitParentIdCollection(this);

					return _parentIds;
				}
			}

			/// <summary>Get the first ancestor/first parent of this commit</summary>
			property GitCommit^ Ancestor
			{
				GitCommit^ get()
				{
					if (!_ancestor && !IsDisposed)
					{
						git_commit *commit;
						int r = git_commit_parent(&commit, Handle, 0);
						
						if (!r)
							_ancestor = gcnew GitCommit(_repository, commit);
					}

					return _ancestor;
				}
			}

			/// <summary>Get an enumerator over this nodes ancestors, starting by the parent of this commit</summary>
			property IEnumerable<GitCommit^>^ Ancestors
			{
				IEnumerable<GitCommit^>^ get();
			}

			/// <summary>Get an enumerator over this nodes ancestors, starting by this node itself</summary>
			property IEnumerable<GitCommit^>^ AncestorsAndSelf
			{
				IEnumerable<GitCommit^>^ get();
			}

			property GitSignature^ Author
			{
				GitSignature ^get();
			}

			property GitSignature^ Committer
			{
				GitSignature ^get();
			}

			property String^ LogMessage
			{
				String^ get();
			}
		};
	}
}
