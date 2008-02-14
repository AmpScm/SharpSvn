// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientEventArgs.h

namespace SharpSvn {
	using System::Collections::ObjectModel::Collection;

	public ref class SvnCancelEventArgs : public SvnEventArgs
	{
		bool _cancel;

	public:
		SvnCancelEventArgs()
		{
		}

	public:
		property bool Cancel
		{
			bool get()
			{
				return _cancel;
			}
			void set(bool value)
			{
				_cancel = value;
			}
		}
	};

	public ref class SvnProgressEventArgs sealed : public SvnEventArgs
	{
		initonly __int64 _progress;
		initonly __int64 _totalProgress;

	public:
		SvnProgressEventArgs(__int64 progress, __int64 totalProgress)
		{
			_progress = progress;
			_totalProgress = totalProgress;
		}

	public:
		property __int64 Progress
		{
			__int64 get()
			{
				return _progress;
			}
		}

		property __int64 TotalProgress
		{
			__int64 get()
			{
				return _totalProgress;
			}
		}
	};

	public ref class SvnCommittingEventArgs sealed : public SvnEventArgs
	{
		AprPool^ _pool;
		const apr_array_header_t *_commitItems;
		bool _cancel;
		String^ _logMessage;
		SvnCommitItemCollection^ _items;
	internal:
		SvnCommittingEventArgs(const apr_array_header_t *commitItems, AprPool^ pool);

	public:
		property bool Cancel
		{
			bool get()
			{
				return _cancel;
			}
			void set(bool value)
			{
				_cancel = value;
			}
		}

		property String^ LogMessage
		{
			String^ get()
			{
				return _logMessage;
			}
			void set(String^ value)
			{
				_logMessage = value;
			}
		}

		property SvnCommitItemCollection^ Items
		{
			SvnCommitItemCollection^ get();
		}

		virtual void Detach(bool keepProperties) override;
	};

	public ref class SvnConflictEventArgs sealed : public SvnCancelEventArgs
	{
		const svn_wc_conflict_description_t* _description;
		SvnConflictChoice _result;

		initonly SvnNodeKind _nodeKind;
		initonly bool _isBinary;
		initonly SvnConflictAction _action;
		initonly SvnConflictReason _reason;
		initonly SvnConflictType _type;

		String^ _propertyName;
		String^ _path;
		String^ _mimeType;

		String^ _baseFile;
		String^ _theirFile;
		String^ _myFile;
		String^ _mergedFile;

		String^ _mergeResult;

	internal:
		SvnConflictEventArgs(const svn_wc_conflict_description_t *description)
		{
			if(!description)
				throw gcnew ArgumentNullException("description");

			_description = description;
			_result = SvnConflictChoice::Postpone;

			_nodeKind = (SvnNodeKind)description->node_kind;
			_action = (SvnConflictAction)description->action;
			_reason = (SvnConflictReason)description->reason;
			_type = (SvnConflictType)description->kind;
			// We ignore the administrative baton for now
		}

	public:
		property SvnConflictChoice Choice
		{
			SvnConflictChoice get()
			{
				return _result;
			}
			void set(SvnConflictChoice value)
			{
				_result = EnumVerifier::Verify(value);
			}
		}

	public:
		property String^ Path
		{
			String^ get()
			{
				if(!_path && _description && _description->path)
					_path = SvnBase::Utf8_PtrToString(_description->path);

				return _path;
			}
		}

		property String^ PropertyName
		{
			String^ get()
			{
				if(!_propertyName && _description && _description->property_name)
					_propertyName = SvnBase::Utf8_PtrToString(_description->property_name);

				return _propertyName;
			}
		}

		property String^ MimeType
		{
			String^ get()
			{
				if(!_mimeType && _description && _description->mime_type)
					_mimeType = SvnBase::Utf8_PtrToString(_description->mime_type);

				return _mimeType;
			}
		}

		property String^ BaseFile
		{
			String^ get()
			{
				if(!_baseFile && _description && _description->base_file)
					_baseFile = SvnBase::Utf8_PtrToString(_description->base_file);

				return _baseFile;
			}
		}

		property String^ TheirFile
		{
			String^ get()
			{
				if(!_theirFile && _description && _description->their_file)
					_theirFile = SvnBase::Utf8_PtrToString(_description->their_file);

				return _theirFile;
			}
		}

		property String^ MyFile
		{
			String^ get()
			{
				if(!_myFile && _description && _description->my_file)
					_myFile = SvnBase::Utf8_PtrToString(_description->my_file);

				return _myFile;
			}
		}

		property String^ MergedFile
		{
			String^ get()
			{
				if(_mergeResult)
					return _mergeResult;

				if(!_mergedFile && _description && _description->merged_file)
					_mergedFile  = SvnBase::Utf8_PtrToString(_description->merged_file);

				return _mergedFile;
			}
			void set(String^ value)
			{
				_mergeResult = String::IsNullOrEmpty(value) ? nullptr : value;
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

	public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
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
				_description = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};

};