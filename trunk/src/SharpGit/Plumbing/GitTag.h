#pragma once
#include "GitObject.h"

struct git_tag {};

namespace SharpGit {
	ref class GitSignature;

	namespace Plumbing {

		public ref class GitTag : GitObject
		{
			GitSignature ^_tagger;
			String ^_message;
			String ^_name;
			GitId ^_targetId;
			GitObject^ _target;

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

		public:
			property String^ Name
			{
				String^ get()
				{
					if (!_name && !IsDisposed)
						_name = Utf8_PtrToString(git_tag_name(Handle));

					return _name;
				}
			}

			property GitSignature^ Tagger
			{
				GitSignature^ get();
			}

			property String^ LogMessage
			{
				String^ get()
				{
					if (!_message && !IsDisposed)
						_message = Utf8_PtrToString(git_tag_message(Handle));

					return _message;
				}
			}

			property GitObject^ Target
			{
				GitObject^ get();
			}

			property GitId^ TargetId
			{
				GitId^ get();
			}
		};
	}
}
