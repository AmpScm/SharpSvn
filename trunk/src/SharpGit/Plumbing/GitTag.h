#pragma once
#include "GitObject.h"

struct git_tag {};

namespace SharpGit {
	namespace Plumbing {

		public ref class GitTag : GitObject
		{
		internal:
			GitTag(GitRepository^ repository, git_tag *handle)
				: GitObject(repository, reinterpret_cast<git_object*>(handle))
			{
			}
				

			property git_tag* Handle
			{
				git_tag* get() new
				{
					return reinterpret_cast<git_tag*>(GitObject::Handle);
				}
			}

		};
	}
}
