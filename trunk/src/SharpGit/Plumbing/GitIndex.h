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
		ref class GitTree;
		ref class GitIndexEntry;

		public ref class GitIndex : public Implementation::GitBase,
									public System::Collections::Generic::ICollection<GitIndexEntry^>
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

		public:
			property bool IsDisposed
			{
				bool get()
				{
					return !_index;
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
				virtual int get()
				{
					if (!_index)
						return -1;
					else
						return git_index_entrycount(_index);
				}
			}

			bool Contains(String ^relPath);

			property GitIndexEntry ^ default[int]
			{
				GitIndexEntry^ get(int index);
			}

			property GitIndexEntry ^ default[String^]
			{
				GitIndexEntry^ get(String ^relativePath);
			}

		public:
			virtual System::Collections::Generic::IEnumerator<GitIndexEntry^>^ GetEnumerator();

		private:
			virtual System::Collections::IEnumerator^ GetObjectEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator
			{
				return GetEnumerator();
			}

			virtual void CollectionClear() sealed = System::Collections::Generic::ICollection<GitIndexEntry^>::Clear
			{
				Clear();
			}

			property bool IsReadOnly
			{
				virtual bool get() sealed = System::Collections::Generic::ICollection<GitIndexEntry^>::IsReadOnly::get
				{
					return false;
				}
			}

			virtual void Add(GitIndexEntry ^entry) sealed = System::Collections::Generic::ICollection<GitIndexEntry^>::Add
			{
				UNUSED(entry);
				throw gcnew InvalidOperationException();
			}

			virtual bool Contains(GitIndexEntry ^entry) sealed = System::Collections::Generic::ICollection<GitIndexEntry^>::Contains
			{
				for each (GitIndexEntry^ i in this)
				{
					if (i == entry)
						return true;
				}
				return false;
			}

			virtual void CopyTo(array<GitIndexEntry^>^ to, int index) sealed = System::Collections::Generic::ICollection<GitIndexEntry^>::CopyTo
			{
				for each (GitIndexEntry^ i in this)
				{
					to[index++] = i;
				}
			}

			virtual bool Remove(GitIndexEntry^ entry) sealed = System::Collections::Generic::ICollection<GitIndexEntry^>::Remove
			{
				int n = 0;

				for each (GitIndexEntry^ i in this)
				{
					if (i == entry)
					{
						return Remove(n);
					}
					n++;
				}
				return false;
			}

		public:
			bool Write();
			bool Write(GitArgs ^args);

			bool Reload();
			bool Reload(GitArgs ^args);

			bool Clear();
			bool Remove(int index);
			bool Remove(String^ relativePath) { return Remove(relativePath, false); }
			bool Remove(String^ relativePath, bool recursive);

			bool CreateTree([Out] GitId^% id);
			bool CreateTree(GitArgs ^args, [Out] GitId^% id);

		public:
			bool Read(GitTree ^replacementTree);
			bool Read(GitTree ^replacementTree, GitArgs ^args);
		};

		public ref class GitIndexEntry sealed : public Implementation::GitBase
		{
			initonly GitIndex ^_index;
			initonly const git_index_entry *_entry;
			String^ _relPath;

		internal:
			GitIndexEntry(GitIndex ^index, const git_index_entry *entry)
			{
				if (! index)
					throw gcnew ArgumentNullException("index");
				else if (! entry)
					throw gcnew ArgumentNullException("entry");

				_index = index;
				_entry = entry;
			}

		public:
			property String^ RelativePath
			{
				String^ get()
				{
					if (!_relPath && !_index->IsDisposed)
						_relPath = Utf8_PtrToString(_entry->path);

					return _relPath;
				}
			}

			property int Stage
			{
				int get()
				{
					if (!_index->IsDisposed)
						return git_index_entry_stage(_entry);
					else
						return -1;
				}
			}

			property __int64 FileSize
			{
				__int64 get()
				{
					if (!_index->IsDisposed)
						return _entry->file_size;
					else
						return -1;
				}
			}

			property GitId^ Id
			{
				GitId^ get()
				{
					if (!_index->IsDisposed)
						return gcnew GitId(_entry->oid);

					return nullptr;
				}
			}
		};
	}
}
