// $Id: SvnStatusEventArgs.h 1024 2009-01-27 19:54:11Z rhuijben $
//
// Copyright 2007-2008 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#pragma once

namespace SharpSvn {
	using namespace System::Collections::ObjectModel;	

	public ref class SvnConflictSource sealed : ISvnOrigin
	{
		const svn_wc_conflict_version_t *_version;
		AprPool^ _pool;

		Uri^ _uri;
		Uri^ _repositoryRoot;
		Uri^ _repositoryPath;
		initonly __int64 _revision;
		initonly SvnNodeKind _nodeKind;

	internal:
		SvnConflictSource(const svn_wc_conflict_version_t *version, AprPool^ pool)
		{
			if (!version)
				throw gcnew ArgumentNullException("version");
			else if (!pool)
				throw gcnew ArgumentNullException("pool");

			_version = version;
			_pool = pool;
			_revision = version->peg_rev;
			_nodeKind = (SvnNodeKind)version->node_kind;
		}

	public:
		property System::Uri^ Uri
		{
			virtual System::Uri^ get() sealed
			{
				if (!_uri && _version && _version->repos_url && _version->path_in_repos && _pool)
					_uri = SvnBase::Utf8_PtrToUri(svn_path_join(_version->repos_url, _version->path_in_repos, _pool->Handle), NodeKind);

				return _uri;
			}
		}

		property System::Uri^ RepositoryRoot
		{
			virtual System::Uri^ get() sealed
			{
				if (!_uri && _version && _version->repos_url && _version->path_in_repos && _pool)
					_uri = SvnBase::Utf8_PtrToUri(_version->repos_url, SvnNodeKind::Directory);

				return _repositoryRoot; 
			}
		}

		/// <summary>Gets the relative uri of the path inside the repository</summary>
		/// <remarks>Does not include an initial '/'. Ends with a '/' if <see cref="NodeKind" /> is <see cref="SvnNodeKind::Directory" />.</remarks>
		property System::Uri^ RepositoryPath
		{
			System::Uri^ get()
			{
				if (!_repositoryPath && RepositoryRoot && Uri)
					_repositoryPath = RepositoryRoot->MakeRelativeUri(Uri);

				return _repositoryPath;
			}
		}

		/// <summary>Gets the revision of <see cref="Uri" /></summary>
		property __int64 Revision
		{
			virtual __int64 get() sealed
			{
				return _revision;
			}
		}

		/// <summary>Gets the <see cref="SvnNodeKind" /> of <see cref="Uri" /></summary>
		property SvnNodeKind NodeKind
		{
			virtual SvnNodeKind get() sealed
			{
				return _nodeKind;
			}
		}

		property SvnUriTarget^ Target
		{
			SvnUriTarget^ get()
			{
				return gcnew SvnUriTarget(Uri, Revision);
			}
		}

		static operator SvnUriTarget^(SvnConflictSource^ value)
		{
			return ((Object^)value != nullptr) ? value->Target : nullptr; 
		}

	private:
		property SvnTarget^ RawTarget
		{
			virtual SvnTarget^ get() sealed = ISvnOrigin::Target::get
			{
				return Target;
			}
		}

	public:
		void Detach()
		{
			Detach(true);
		}
	protected public:
		void Detach(bool keepProperties)
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(Uri);
					GC::KeepAlive(RepositoryRoot);
				}
			}
			finally
			{
				_version = nullptr;
				_pool = nullptr;
			}
		}
	};

	public ref class SvnConflictData sealed
	{
		const svn_wc_conflict_description_t *_description;
		AprPool^ _pool;

		initonly SvnNodeKind _nodeKind;
		initonly bool _isBinary;
		initonly SvnConflictAction _action;
		initonly SvnConflictReason _reason;
		initonly SvnConflictType _type;
		initonly SvnOperation _operation;

		String^ _propertyName;
		String^ _path;
		String^ _mimeType;

		String^ _baseFile;
		String^ _theirFile;
		String^ _myFile;
		String^ _mergedFile;

		String^ _mergeResult;

		SvnConflictSource^ _leftOrigin;
		SvnConflictSource^ _rightOrigin;

	internal:
		SvnConflictData(const svn_wc_conflict_description_t *description, AprPool^ pool)
		{
			if (!description)
				throw gcnew ArgumentNullException("description");
			else if (!pool)
				throw gcnew ArgumentNullException("pool");

			_description = description;
			_pool = pool;

			_isBinary = (description->is_binary != 0);
			_nodeKind = (SvnNodeKind)description->node_kind;
			_action = (SvnConflictAction)description->action;
			_reason = (SvnConflictReason)description->reason;
			_type = (SvnConflictType)description->kind;
			_operation = (SvnOperation)description->operation;
		}

	public:
		property String^ Name
		{
			String^ get()
			{
				if (!_path && _description && _description->path && _pool)
					_path = SvnBase::Utf8_PathPtrToString(_description->path, _pool);

				return _path;
			}
		}

		property String^ PropertyName
		{
			String^ get()
			{
				if (!_propertyName && _description && _description->property_name)
					_propertyName = SvnBase::Utf8_PtrToString(_description->property_name);

				return _propertyName;
			}
		}

		property String^ MimeType
		{
			String^ get()
			{
				if (!_mimeType && _description && _description->mime_type)
					_mimeType = SvnBase::Utf8_PtrToString(_description->mime_type);

				return _mimeType;
			}
		}

		property String^ BaseFile
		{
			String^ get()
			{
				if (!_baseFile && _description && _description->base_file && _pool)
					_baseFile = SvnBase::Utf8_PathPtrToString(_description->base_file, _pool);

				return _baseFile;
			}
		}

		property String^ TheirFile
		{
			String^ get()
			{
				if (!_theirFile && _description && _description->their_file && _pool)
					_theirFile = SvnBase::Utf8_PathPtrToString(_description->their_file, _pool);

				return _theirFile;
			}
		}

		property String^ MyFile
		{
			String^ get()
			{
				if (!_myFile && _description && _description->my_file && _pool)
					_myFile = SvnBase::Utf8_PathPtrToString(_description->my_file, _pool);

				return _myFile;
			}
		}

		property String^ MergedFile
		{
			String^ get()
			{
				if (_mergeResult)
					return _mergeResult;

				if (!_mergedFile && _description && _description->merged_file && _pool)
					_mergedFile  = SvnBase::Utf8_PathPtrToString(_description->merged_file, _pool);

				return _mergedFile;
			}
			void set(String^ value)
			{
				if (String::IsNullOrEmpty(value) && (value != MergedFile))
					throw gcnew InvalidOperationException("Only settable with valid filename");

				_mergeResult = value;
			}
		}

		property bool IsBinary
		{
			bool get()
			{
				return _isBinary;
			}
		}

		property SvnConflictAction ConflictAction
		{
			SvnConflictAction get()
			{
				return _action;
			}
		}

		property SvnConflictReason ConflictReason
		{
			SvnConflictReason get()
			{
				return _reason;
			}
		}

		property SvnConflictType ConflictType
		{
			SvnConflictType get()
			{
				return _type;
			}
		}

		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return _nodeKind;
			}
		}

		/// <summary>Gets the operation creating the tree conflict</summary>
		property SvnOperation Operation
		{
			SvnOperation get()
			{
				return _operation;
			}
		}

		property SvnConflictSource^ LeftSource
		{
			SvnConflictSource^ get()
			{
				if (!_leftOrigin && _description && _description->src_left_version && _pool)
					_leftOrigin = gcnew SvnConflictSource(_description->src_left_version, _pool);

				return _leftOrigin;
			}
		}

		property SvnConflictSource^ RightSource
		{
			SvnConflictSource^ get()
			{
				if (!_rightOrigin && _description && _description->src_right_version && _pool)
					_rightOrigin = gcnew SvnConflictSource(_description->src_right_version, _pool);

				return _rightOrigin;
			}
		}

	public:
		void Detach()
		{
			Detach(true);
		}

	protected public:
		virtual void Detach(bool keepProperties)
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(Name);
					GC::KeepAlive(PropertyName);
					GC::KeepAlive(MimeType);
					GC::KeepAlive(BaseFile);
					GC::KeepAlive(TheirFile);
					GC::KeepAlive(MyFile);
					GC::KeepAlive(MergedFile);

					GC::KeepAlive(LeftSource);
					GC::KeepAlive(RightSource);
				}

				if (_leftOrigin)
					_leftOrigin->Detach(keepProperties);
				if (_rightOrigin)
					_rightOrigin->Detach(keepProperties);
			}
			finally
			{
				_description = nullptr;
				_pool = nullptr;
			}
		}
	};
}
