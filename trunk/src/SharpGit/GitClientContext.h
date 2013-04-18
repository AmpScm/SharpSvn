#pragma once

using namespace System;
#define UNUSED(x) (x)

namespace SharpGit {

	namespace Implementation {
		ref class GitPool;

		public ref class GitBase : public System::MarshalByRefObject
		{
		internal:
			GitBase() {}

		internal:
			static System::String^ Utf8_PtrToString(const char *ptr);
			static System::String^ Utf8_PtrToString(const char *ptr, int length);
			static System::String ^StringFromDirentNoPool(const char *dirent);
			static System::String ^StringFromDirent(const char *dirent, GitPool ^pool);
		};

		private ref class GitPool
		{
		private:
			apr_pool_t *_pool;
			GitPool ^_parent;

			static GitPool();

			~GitPool();
			!GitPool();

		public:
			GitPool(GitPool ^parent)
			{
				_parent = parent;
				_pool = svn_pool_create(parent->Handle);
			}

			GitPool()
			{
				_pool = svn_pool_create(nullptr);
			}

		public:
			property apr_pool_t *Handle
			{
				apr_pool_t *get()
				{
					if (!_pool)
						throw gcnew InvalidOperationException();
					return _pool;
				}
			}

		public:
			char *Alloc(size_t sz);
			const char *AllocDirent(String ^path);
		};
	}

	public ref class GitClientContext : public Implementation::GitBase
	{
	internal:
		SharpGit::Implementation::GitPool _pool;
		GitClientContext() {}

	};

	public ref class GitArgs abstract
	{
		static GitArgs()
		{}

	public:

	internal:
		bool HandleGitError(Object^ q, int r)
		{
			UNUSED(q);
			if (r != 0)
				throw gcnew Exception(String::Format("Git Failure: {0}", r));

			return true;
		}
	};

	public ref class GitClientArgs abstract : public GitArgs
	{
	};

	private ref class GitNoArgs sealed : GitArgs
	{
	};

}