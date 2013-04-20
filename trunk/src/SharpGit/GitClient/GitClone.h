#pragma once
#include "GitClientContext.h"

namespace SharpGit {

	public ref class GitClientRemoteArgs abstract : public GitClientArgs
	{
	};

	public ref class GitCloneArgs : public GitClientRemoteArgs
	{
	};

}
