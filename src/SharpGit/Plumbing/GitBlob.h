#pragma once
#include "GitObject.h"

struct git_blob {};

namespace SharpGit {
	namespace Plumbing {

		public ref class GitBlob : GitObject
		{
		internal:
			GitBlob(GitRepository^ repository, git_blob *handle)
				: GitObject(repository, reinterpret_cast<git_object*>(handle))
			{
			}
				

			property git_blob* Handle
			{
				git_blob* get() new
				{
					return reinterpret_cast<git_blob*>(GitObject::Handle);
				}
			}

		};
	}
}
