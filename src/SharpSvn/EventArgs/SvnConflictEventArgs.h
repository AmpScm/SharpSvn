// $Id$
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

// Included from SvnClientEventArgs.h

namespace SharpSvn {

	public ref class SvnConflictEventArgs sealed : public SvnCancelEventArgs
	{
		const svn_wc_conflict_description_t* _description;
		SvnAccept _result;

		initonly SvnNodeKind _nodeKind;
		initonly bool _isBinary;
		initonly SvnConflictAction _action;
		initonly SvnConflictReason _reason;
		initonly SvnConflictType _type;

		AprPool^ _pool;
		String^ _propertyName;
		String^ _path;
		String^ _mimeType;

		String^ _baseFile;
		String^ _theirFile;
		String^ _myFile;
		String^ _mergedFile;

		String^ _mergeResult;

	internal:
		SvnConflictEventArgs(const svn_wc_conflict_description_t *description, AprPool^ pool)
		{
			if (!description)
				throw gcnew ArgumentNullException("description");
			else if (!pool)
				throw gcnew ArgumentNullException("pool");

			_description = description;
			_pool = pool;
			_result = SvnAccept::Postpone;

			_isBinary = (description->is_binary != 0);
			_nodeKind = (SvnNodeKind)description->node_kind;
			_action = (SvnConflictAction)description->action;
			_reason = (SvnConflictReason)description->reason;
			_type = (SvnConflictType)description->kind;			
			// We ignore the administrative baton for now
		}

	public:
		property SvnAccept Choice
		{
			SvnAccept get()
			{
				return _result;
			}
			void set(SvnAccept value)
			{
				_result = EnumVerifier::Verify(value);
			}
		}

	public:
		property String^ Path
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

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(Path);
					GC::KeepAlive(PropertyName);
					GC::KeepAlive(MimeType);
					GC::KeepAlive(BaseFile);
					GC::KeepAlive(TheirFile);
					GC::KeepAlive(MyFile);
					GC::KeepAlive(MergedFile);
				}
			}
			finally
			{
				_pool = nullptr;
				_description = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};
}

