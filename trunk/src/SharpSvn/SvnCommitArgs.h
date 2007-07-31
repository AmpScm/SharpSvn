#pragma once
#include "AprArray.h"
namespace SharpSvn {
	using namespace SharpSvn::Apr;
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IList;

	ref class SvnCommitItem;

	public ref class SvnCommitInfo
	{
		initonly __int64 _revision;
		initonly DateTime _date;
		initonly String^ _author;
		initonly String^ _postCommitError;

	internal:
		SvnCommitInfo(const svn_commit_info_t *commitInfo);

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

	public ref class SvnCommitItem : public SvnBase
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
					_path = Utf8_PtrToString(_info->path);
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
					_uri = gcnew System::Uri(Utf8_PtrToString(_info->url));

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
					_copyFromUri = gcnew System::Uri(Utf8_PtrToString(_info->copyfrom_url));

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

	ref class SvnCommitItemMarshaller sealed : public SvnBase, public IItemMarshaller<SvnCommitItem^>
	{
	public:
		SvnCommitItemMarshaller()
		{}

		property int ItemSize
		{
			virtual int get()
			{
				return sizeof(svn_client_commit_item2_t*);
			}
		}

		virtual void Write(SvnCommitItem^ value, void* ptr, AprPool^ pool)
		{
			UNUSED_ALWAYS(value);
			UNUSED_ALWAYS(ptr);
			UNUSED_ALWAYS(pool);
			throw gcnew NotImplementedException();
		}

		virtual SvnCommitItem^ Read(const void* ptr)
		{
			const svn_client_commit_item3_t** ppcCommitItem = (const svn_client_commit_item3_t**)ptr;

			return gcnew SvnCommitItem(*ppcCommitItem);
		}
	};
}