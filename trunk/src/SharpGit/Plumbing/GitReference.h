#pragma once
#include "GitClientContext.h"

namespace SharpGit {
	namespace Plumbing {


		public ref class GitReference : public Implementation::GitBase
		{
		private:
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
			GitReference(git_reference *handle)
			{
				if (! handle)
					throw gcnew ArgumentNullException("handle");

				_reference = handle;
			}

			property git_reference* Handle
			{
				git_reference* get()
				{
					if (!_reference)
						throw gcnew InvalidOperationException();
					return _reference;
				}
			}

		public:
			property bool IsDisposed
			{
				bool get()
				{
					return !_reference;
				}
			}

		};
	}
}