#pragma once
#include <vcclr.h>

using namespace System;
#define UNUSED(x) (x)

namespace SharpGit {

	namespace Implementation {
		ref class GitPool;

		template<class T> 
		struct GitRoot : gcroot<T>
		{
			apr_pool_t *_pool;
			const char *_wcPath;
		public:
			GitRoot(T value)
				: gcroot(value)
			{
				_pool = nullptr;
				_wcPath = nullptr;
			}

			GitRoot(T value, const char *wcPath, GitPool ^pool)
				: gcroot(value)
			{
				_wcPath = wcPath;
				_pool = pool->Handle;
			}

			GitRoot(T value, GitPool ^pool)
				: gcroot(value)
			{
				_wcPath = nullptr;
				_pool = pool->Handle;
			}

			GitRoot(void *batonValue)
				: gcroot(*reinterpret_cast<GitRoot<T>*>(batonValue))
			{
				_pool = reinterpret_cast<GitRoot<T>*>(batonValue)->_pool;
				_wcPath = reinterpret_cast<GitRoot<T>*>(batonValue)->_wcPath;
			}

		public:
			void *GetBatonValue() { return (void*)this; }
			apr_pool_t *GetPool() { return _pool; }
			const char *GetWcPath() { return _wcPath; }
		};

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

			GitPool(GitPool %parent)
			{
				_parent = %parent;
				_pool = svn_pool_create(parent.Handle);
			}

			GitPool()
			{
				_pool = svn_pool_create(nullptr);
			}

			GitPool(apr_pool_t *parent)
			{
				_pool = svn_pool_create(parent);
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
			const char *AllocRelpath(String ^relPath);
			const char *AllocString(String^ value);
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
		literal int WrappedError = 1099001;
		Exception ^_ex;

		bool HandleGitError(Object^ q, int r)
		{
			if (_ex && r == WrappedError)
			{
				try
				{
					throw gcnew Exception(String::Format("Wrapped Exception: {0}", _ex), _ex);
				}
				finally
				{
					_ex = nullptr;
				}
			}

			UNUSED(q);
			_ex = nullptr;

			switch (r)
			{
			case GIT_OK /* 0 */:
				return true;

			case GIT_ERROR:
			case GIT_ENOTFOUND:
			case GIT_EEXISTS:
			case GIT_EAMBIGUOUS:
			case GIT_EBUFS:

			case GIT_PASSTHROUGH:
			case GIT_REVWALKOVER:
			case GITERR_OS:
			case GITERR_INVALID:
			case GITERR_REFERENCE:
			case GITERR_ZLIB:
			case GITERR_REPOSITORY:
			case GITERR_CONFIG:
			case GITERR_REGEX:
			case GITERR_ODB:
			case GITERR_INDEX:
			case GITERR_OBJECT:
			case GITERR_NET:
			case GITERR_TAG:
			case GITERR_TREE:
			case GITERR_INDEXER:
				{
					const git_error *info = giterr_last();

					try
					{
						if (info)
							throw gcnew Exception(String::Format("Git Error: {0}/{1}: {2}", r, info->klass, GitBase::Utf8_PtrToString(info->message)));
						else
							throw gcnew Exception(String::Format("Git Error: {0}", r));
					}
					finally
					{
						if (info)
							giterr_clear();
					}
				}
			default:
				throw gcnew Exception(String::Format("Unknown git Error: {0}", r));
			}

			//return true;
		}

		int WrapException(Exception ^e)
		{
			if (e)
			{
				System::Diagnostics::Debug::WriteLine("Wrapping: {0}", e);
				_ex = e;
				return WrappedError;
			}

			return 0;
		}
	};

	public ref class GitClientArgs abstract : public GitArgs
	{
	};

	private ref class GitNoArgs sealed : GitArgs
	{
	};

}