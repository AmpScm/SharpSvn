#pragma once
#include "GitClientContext.h"

namespace SharpGit {
	namespace Plumbing {


		public ref class GitReference : public Implementation::GitBase
		{
		private:
			initonly GitRepository ^_repository;
			git_reference *_reference;

			!GitReference()
			{
				if (_reference)
				{
					try
					{
						git_reference_free(_reference);
					}
					finally
					{
						_reference = nullptr;
					}
				}
			}

			~GitReference()
			{
				try
					{
						git_reference_free(_reference);
					}
					finally
					{
						_reference = nullptr;
					}
			}

		internal:
			GitReference(GitRepository ^repository, git_reference *handle)
			{
				if (! repository)
					throw gcnew ArgumentNullException("repository");
				if (! handle)
					throw gcnew ArgumentNullException("handle");

				_repository = repository;
				_reference = handle;
			}

			property git_reference* Handle
			{
				git_reference* get()
				{
					if (IsDisposed)
						throw gcnew InvalidOperationException();
					return _reference;
				}
			}

		private:
			String^ _name;

		public:
			property bool IsDisposed
			{
				bool get();
			}

			property String^ Name
			{
				String^ get()
				{
					if (!_name && !IsDisposed)
					{
						_name = GitBase::Utf8_PtrToString(git_reference_name(_reference));
					}

					return _name;
				}
			}

			bool Delete();
			bool Delete(GitArgs ^args);
		};
	}
}