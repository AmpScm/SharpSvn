#pragma once
#include "GitClientContext.h"

namespace SharpGit {
	namespace Plumbing {


		public ref class GitConfiguration : public Implementation::GitBase
		{
		private:
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
			GitConfiguration(git_config *config)
			{
				if (! config)
					throw gcnew ArgumentNullException("config");

				_config = config;
			}

			property git_config* Handle
			{
				git_config* get()
				{
					if (!_config)
						throw gcnew InvalidOperationException();
					return _config;
				}
			}


		};
	}
}