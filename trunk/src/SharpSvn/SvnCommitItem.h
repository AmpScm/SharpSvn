// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "AprArray.h"

namespace SharpSvn {
	using namespace SharpSvn::Apr;
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IList;

	ref class SvnCommitItem;

	public ref class SvnCommitInfo sealed
	{
		initonly __int64 _revision;
		initonly DateTime _date;
		initonly String^ _author;
		initonly String^ _postCommitError;

	internal:
		SvnCommitInfo(const svn_commit_info_t *commitInfo, AprPool^ pool);

	public:
		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}

		property DateTime Date
		{
			DateTime get()
			{
				return _date;
			}
		}

		property String^ Author
		{
			String^ get()
			{
				return _author;
			}
		}

		/// <summary>error message from post-commit hook, or NULL</summary>
		property String^ PostCommitError
		{
			String^ get()
			{
				return _postCommitError;
			}
		}
	};

	public ref class SvnCommitItem sealed
	{
		const svn_client_commit_item3_t *_info;
		String^ _path;
		String^ _fullPath;
		initonly SvnNodeKind _nodeKind;
		Uri^ _uri;
		initonly __int64 _revision;
		Uri^ _copyFromUri;
		initonly __int64 _copyFromRevision;

	internal:
		SvnCommitItem(const svn_client_commit_item3_t *commitItemInfo);

	public:
		property String^ Path
		{
			String^ get()
			{
				if(!_path && _info)
					_path = SvnBase::Utf8_PtrToString(_info->path);
				return _path;
			}
		}

		property String^ FullPath
		{
			String^ get()
			{
				if(!_fullPath && Path)
					_fullPath = System::IO::Path::GetFullPath(Path);

				return _fullPath;
			}
		}

		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return _nodeKind;
			}
		}

		property System::Uri^ Uri
		{
			System::Uri^ get()
			{
				if(!_uri && _info && _info->url)
					_uri = gcnew System::Uri(SvnBase::Utf8_PtrToString(_info->url));

				return _uri;
			}
		}

		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}

		property System::Uri^ CopyFromUri
		{
			System::Uri^ get()
			{
				if(!_copyFromUri && _info && _info->copyfrom_url)
					_copyFromUri = gcnew System::Uri(SvnBase::Utf8_PtrToString(_info->copyfrom_url));

				return _copyFromUri;
			}
		}

		property __int64 CopyFromRevision
		{
			__int64 get()
			{
				return _copyFromRevision;
			}
		}

		void Detach(bool keepProperties);
	};
}