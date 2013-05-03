#pragma once
#include "GitClientContext.h"

namespace SharpGit {
	namespace Plumbing {


		public ref class GitConfiguration : public Implementation::GitBase
		{
		private:
			initonly GitRepository^ _repository;
			git_config *_config;

			!GitConfiguration()
			{
				if (_config)
				{
					try
					{
						git_config_free(_config);
					}
					finally
					{
						_config = nullptr;
					}
				}
			}

			~GitConfiguration()
			{
				try
					{
						git_config_free(_config);
					}
					finally
					{
						_config = nullptr;
					}
			}

		internal:
			GitConfiguration(GitRepository^ repository, git_config *config)
			{
				if (! repository)
					throw gcnew ArgumentNullException("repository");
				else if (! config)
					throw gcnew ArgumentNullException("config");

				_repository = repository;
				_config = config;
			}

			// TODO: Support global configuration instance

			property git_config* Handle
			{
				git_config* get()
				{
					if (IsDisposed)
						throw gcnew InvalidOperationException();

					return _config;
				}
			}

		public:
			property bool IsDisposed
			{
				bool get();
			}

			bool Reload();
			bool Reload(GitArgs ^args);

		public:
			bool TryGetInt32(String ^key, [Out] int %value);
			bool TryGetInt64(String ^key, [Out] __int64 %value);
			bool TryGetString(String ^key, [Out] String ^%value);
			bool TryGetBoolean(String ^key, [Out] bool ^value);

		public:
			bool Set(String ^key, int value);
			bool Set(String ^key, __int64 value);
			bool Set(String ^key, String ^value);
			bool Set(String ^key, bool value);
			bool Set(String ^key, GitArgs ^args, int value);
			bool Set(String ^key, GitArgs ^args, __int64 value);
			bool Set(String ^key, GitArgs ^args, String ^value);
			bool Set(String ^key, GitArgs ^args, bool value);

			bool Delete(String ^key);
			bool Delete(String ^key, GitArgs ^args);
		};
	}
}