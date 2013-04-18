#pragma once

namespace SharpGit {

	public ref class GitEventArgs abstract : public EventArgs
	{
	};

	public ref class GitClientEventArgs abstract : public GitEventArgs
	{
	};
}

#include "GitStatusEventArgs.h"