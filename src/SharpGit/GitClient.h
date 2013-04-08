#pragma once

#include "GitClientContext.h"

namespace SharpGit {

	public ref class GitClient : GitClientContext
	{

	public:
		static property System::Version^ Version
		{
			System::Version^ get();
		}

		static property System::Version^ SharpGitVersion
		{
			System::Version^ get();
		}

	private:
		~GitClient();
	};

}