#pragma once
#include "GitClientContext.h"

namespace SharpGit {
	namespace Plumbing {


		public ref class GitObjectDatabase : public Implementation::GitBase
		{
		private:
			git_odb *_odb;

			!GitObjectDatabase()
			{
				if (_odb)
				{
					try
					{
						git_odb_free(_odb);
					}
					finally
					{
						_odb = nullptr;
					}
				}
			}

			~GitObjectDatabase()
			{
				try
					{
						git_odb_free(_odb);
					}
					finally
					{
						_odb = nullptr;
					}
			}

		internal:
			GitObjectDatabase(git_odb *odb)
			{
				if (! odb)
					throw gcnew ArgumentNullException("odb");

				_odb = odb;
			}

			property git_odb* Handle
			{
				git_odb* get()
				{
					if (!_odb)
						throw gcnew InvalidOperationException();
					return _odb;
				}
			}

		public:
			property bool IsDisposed
			{
				bool get()
				{
					return _odb != nullptr;
				}
			}

		};
	}
}