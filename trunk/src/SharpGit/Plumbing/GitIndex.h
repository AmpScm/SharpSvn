#pragma once
#include "GitClientContext.h"

namespace SharpGit {
	namespace Plumbing {


		public ref class GitIndex : public Implementation::GitBase
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


		};
	}
}