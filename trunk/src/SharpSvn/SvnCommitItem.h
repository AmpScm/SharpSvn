// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "AprArray.h"

namespace SharpSvn {
	using namespace SharpSvn::Implementation;
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IList;

	ref class SvnCommitItem;
	ref class SvnClient;
	ref class SvnClientArgs;

	public ref class SvnCommitResult sealed : public SvnCommandResult
	{
		initonly __int64 _revision;
		initonly DateTime _date;
		initonly String^ _author;
		initonly String^ _postCommitError;

	internal:
		static SvnCommitResult^ Create(SvnClient^ client, SvnClientArgs^ args, const svn_commit_info_t *commitInfo, AprPool^ pool);

	private:
		SvnCommitResult(const svn_commit_info_t *commitInfo, AprPool^ pool);

	public:
		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}

		property DateTime Time
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

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return Revision.GetHashCode();
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
				if (!_path && _info)
					_path = SvnBase::Utf8_PtrToString(_info->path);
				return _path;
			}
		}

		property String^ FullPath
		{
			String^ get()
			{
				if (!_fullPath && Path)
					_fullPath = SvnTools::GetNormalizedFullPath(Path);

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
				if (!_uri && _info && _info->url)
					_uri = SvnBase::Utf8_PtrToUri(_info->url, _nodeKind);

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
				if (!_copyFromUri && _info && _info->copyfrom_url)
					_copyFromUri = SvnBase::Utf8_PtrToUri(_info->copyfrom_url, _nodeKind);

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

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return Revision.GetHashCode();
		}

		void Detach()
		{
			Detach(true);
		}

	protected public:
		void Detach(bool keepProperties);
	};

	namespace Implementation {
		public ref class SvnCommitItemCollection sealed : public System::Collections::ObjectModel::KeyedCollection<String^, SvnCommitItem^>
		{
		internal:
			SvnCommitItemCollection(IList<SvnCommitItem^>^ items)
			{
				for each (SvnCommitItem^ i in items)
				{
					Add(i);
				}
			}

		protected:
			virtual String^ GetKeyForItem(SvnCommitItem^ item) override
			{
				if (!item)
					throw gcnew ArgumentNullException("item");

				return item->Path;
			}
		};

	}
}
