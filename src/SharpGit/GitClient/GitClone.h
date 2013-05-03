#pragma once
#include "GitCheckOut.h"

namespace SharpGit {

	public ref class GitCloneArgs : public GitCheckOutArgs
	{
		bool _bare;
	internal:
		const git_clone_options * MakeCloneOptions(GitPool ^pool);

	public:
		GitCloneArgs()
		{
			Create = true;
		}

		property bool CreateBareRepository
		{
			bool get()
			{
				return _bare;
			}
			void set(bool value)
			{
				_bare = value;
			}
		}
	};

}
