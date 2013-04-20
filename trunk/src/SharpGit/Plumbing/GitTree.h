#pragma once
#include "GitClientContext.h"

struct git_tree {};
struct git_object {};

namespace SharpGit {
	namespace Plumbing {


		public ref class GitTree : public Implementation::GitBase
		{
		private:
			git_tree *_tree;

			!GitTree()
			{
				if (_tree)
				{
					try
					{
						git_tree_free(_tree);
					}
					finally
					{
						_tree = nullptr;
					}
				}
			}

			~GitTree()
			{
				try
					{
						git_tree_free(_tree);
					}
					finally
					{
						_tree = nullptr;
					}
			}

		internal:
			GitTree(git_tree *odb)
			{
				if (! odb)
					throw gcnew ArgumentNullException("odb");

				_tree = odb;
			}

			property git_tree* Handle
			{
				git_tree* get()
				{
					if (!_tree)
						throw gcnew InvalidOperationException();
					return _tree;
				}
			}

		public:
			property bool IsDisposed
			{
				bool get()
				{
					return !_tree;
				}
			}
		};
	}
}