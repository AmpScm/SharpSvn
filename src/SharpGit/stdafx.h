#pragma once

#pragma warning(push)
#pragma warning(disable: 4634 4635)
#include <git2.h>
#include <apr.h>
#include <apr_file_io.h>
#include <svn_dirent_uri.h>
#include <svn_io.h>
#include <svn_pools.h>
#include <svn_utf.h>
#pragma warning(pop)

#include <assert.h>

#pragma warning(disable: 4706) // assignment within conditional expression

using System::String;

namespace SharpGit {
	namespace Implementation {
		ref class GitPool;
		ref class GitBase;
	}

	namespace Plumbing {
		ref class GitRepository;
		ref class GitIndex;
	}
}

using SharpGit::Implementation::GitBase;
using SharpGit::Implementation::GitPool;
using SharpGit::Plumbing::GitRepository;

#include "GitId.h"