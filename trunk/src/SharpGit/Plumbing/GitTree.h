#pragma once
#include "GitClientContext.h"

struct git_tree {};
struct git_object {};

namespace SharpGit {
	namespace Plumbing {

		ref class GitTreeEntry;

		public ref class GitTree sealed : public Implementation::GitBase,
										  public System::Collections::Generic::ICollection<GitTreeEntry^>
		{
		private:
			git_tree *_tree;

			!GitTree()
			{
				if (_tree)
				{
					try
					{
						git_tree_free(_tree);
					}
					finally
					{
						_tree = nullptr;
					}
				}
			}

			~GitTree()
			{
				try
					{
						git_tree_free(_tree);
					}
					finally
					{
						_tree = nullptr;
					}
			}

		internal:
			GitTree(git_tree *handle)
			{
				if (! handle)
					throw gcnew ArgumentNullException("handle");

				_tree = handle;
			}

			property git_tree* Handle
			{
				git_tree* get()
				{
					if (!_tree)
						throw gcnew InvalidOperationException();
					return _tree;
				}
			}

		public:
			property bool IsDisposed
			{
				bool get()
				{
					return !_tree;
				}
			}

			property GitId^ Id
			{
				GitId^ get()
				{
					if (IsDisposed)
						return nullptr;

					return gcnew GitId(git_tree_id(_tree));
				}
			}

			property int Count
			{
				virtual int get()
				{
					if (IsDisposed)
						return 0;

					return git_tree_entrycount(Handle);
				}
			}

			property GitTreeEntry^ default[int]
			{
				GitTreeEntry^ get(int index);
			}

			property GitTreeEntry^ default[String^]
			{
				GitTreeEntry^ get(String^ relPath);
			}

			virtual IEnumerator<GitTreeEntry^>^ GetEnumerator();

			bool Contains(String ^relPath);

		private:
			virtual bool Contains(GitTreeEntry^ entry) sealed  = ICollection<GitTreeEntry^>::Contains
			{
				for each(GitTreeEntry^ i in this)
				{
					if (Equals(i, entry))
						return true;
				}
				return false;
			}

			virtual System::Collections::IEnumerator^ GetObjectEnumerator() sealed = System::Collections::IEnumerable::GetEnumerator
			{
				return GetEnumerator();
			}

			property bool IsReadOnly
			{
				virtual bool get() sealed = ICollection<GitTreeEntry^>::IsReadOnly::get
				{
					return true;
				}
			}

			virtual void Add(GitTreeEntry^ entry) sealed = ICollection<GitTreeEntry^>::Add
			{
				UNUSED(entry);
				throw gcnew InvalidOperationException();
			}

			virtual void Clear() sealed = ICollection<GitTreeEntry^>::Clear
			{
				throw gcnew InvalidOperationException();
			}

			virtual void CopyTo(array<GitTreeEntry^> ^toArray, int index) sealed = ICollection<GitTreeEntry^>::CopyTo
			{
				for each(GitTreeEntry^ i in this)
				{
					toArray[index++] = i;
				}
			}

			virtual bool Remove(GitTreeEntry^ entry) sealed = ICollection<GitTreeEntry^>::Remove
			{
				UNUSED(entry);
				throw gcnew InvalidOperationException();
			}
		};

		public ref class GitTreeEntry sealed : public GitBase
		{
			initonly GitTree ^_tree;
			initonly const git_tree_entry *_entry;
			String^ _name;
			GitId^ _id;
			ICollection<GitTreeEntry^>^ _children;

		internal:
			GitTreeEntry(GitTree ^tree, const git_tree_entry *entry)
			{
				if (! tree)
					throw gcnew ArgumentNullException("tree");
				else if (! entry)
					throw gcnew ArgumentNullException("entry");

				_tree = tree;
				_entry = entry;
			}

		public:
			property String^ Name
			{
				String^ get()
				{
					if (!_name && !_tree->IsDisposed)
						_name = Utf8_PtrToString(git_tree_entry_name(_entry));

					return _name;
				}
			}

			property GitId^ Id
			{
				GitId^ get()
				{
					if (!_id && !_tree->IsDisposed)
						_id = gcnew GitId(git_tree_entry_id(_entry));

					return _id;
				}
			}

			property GitObjectKind Kind
			{
				GitObjectKind get()
				{
					if (!_tree->IsDisposed)
						return (GitObjectKind)git_tree_entry_type(_entry);

					return GitObjectKind::Bad;
				}
			}

		public:
			property ICollection<GitTreeEntry^>^ Children
			{
				ICollection<GitTreeEntry^>^ get();
			}
		};
	}
}