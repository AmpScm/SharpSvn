#pragma once
#include "GitClientContext.h"

namespace SharpGit {
	ref class GitTagArgs;
	namespace Plumbing {

		ref class GitRepository;

		[System::Diagnostics::DebuggerDisplay("{Kind}: {Id}")]
		public ref class GitObject : public Implementation::GitBase
		{
		private:
			git_object *_obj;
			GitId ^_id;
			initonly GitRepository^ _repository;

			!GitObject()
			{
				if (_obj)
				{
					try
					{
						git_object_free(_obj);
					}
					finally
					{
						_obj = nullptr;
					}
				}
			}

			~GitObject()
			{
				try
					{
						git_object_free(_obj);
					}
					finally
					{
						_obj = nullptr;
					}
			}

		private protected:
			GitObject(GitRepository^ repository, git_object *handle)
			{
				if (! repository)
					throw gcnew ArgumentNullException("repository");
				else if (! handle)
					throw gcnew ArgumentNullException("handle");

				_repository = repository;
				_obj = handle;
			}

			property git_object* Handle
			{
				git_object* get()
				{
					if (IsDisposed)
						throw gcnew InvalidOperationException();
					return _obj;
				}
			}

		public:
			property bool IsDisposed
			{
				bool get();
			}

			property GitRepository^ Repository
			{
				GitRepository^ get()
				{
					return _repository;
				}
			}

			property GitId^ Id
			{
				GitId^ get();
			}

			property GitObjectKind Kind
			{
				GitObjectKind get()
				{
					if (! IsDisposed)
						return (GitObjectKind)git_object_type(_obj);
					else
						return GitObjectKind::Bad;
				}
			}

			generic<typename T> where T : GitObject
			T Peel();

			bool Tag(String ^tagName, GitTagArgs ^args);
			bool Tag(String ^tagName, GitTagArgs ^args, [Out] GitId ^%id);

		protected public:
			generic<typename T> where T : GitObject
			static GitObjectKind ObjectKind();

		internal:
			static GitObject^ Create(GitRepository^ repository, git_object *handle);
		};
	}
}