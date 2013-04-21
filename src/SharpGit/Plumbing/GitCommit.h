#pragma once
#include "GitClientContext.h"

namespace SharpGit {
	namespace Plumbing {


		public ref class GitCommit : public Implementation::GitBase
		{
		private:
			git_commit *_commit;

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
			GitCommit(git_commit *handle)
			{
				if (! handle)
					throw gcnew ArgumentNullException("handle");

				_commit = handle;
			}

			property git_commit* Handle
			{
				git_commit* get()
				{
					if (!_commit)
						throw gcnew InvalidOperationException();
					return _commit;
				}
			}

		public:
			property bool IsDisposed
			{
				bool get()
				{
					return !_commit;
				}
			}

		};
	}
}
