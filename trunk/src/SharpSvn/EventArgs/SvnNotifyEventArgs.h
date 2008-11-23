// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

#include "SvnLockInfo.h"

namespace SharpSvn {

	public ref class SvnNotifyEventArgs sealed : public SvnEventArgs
	{
		const svn_wc_notify_t *_notify;
		initonly SvnNotifyAction _action;
		initonly SvnNodeKind _nodeKind;
		initonly SvnNotifyState _contentState;
		initonly SvnNotifyState _propertyState;
		initonly SvnLockState _lockState;
		initonly __int64 _revision;
		initonly SvnCommandType _commandType;
		SvnLockInfo^ _lock;
		String^ _changelistName;
		SvnMergeRange^ _mergeRange;
		bool _pathIsUri;
		bool _mimeTypeIsBinary;

	internal:
		SvnNotifyEventArgs(const svn_wc_notify_t *notify, SvnCommandType commandType)
		{
			if (!notify)
				throw gcnew ArgumentNullException("notify");

			_notify = notify;
			_action = (SvnNotifyAction)notify->action;
			_nodeKind = (SvnNodeKind)notify->kind;
			_contentState = (SvnNotifyState)notify->content_state;
			_propertyState = (SvnNotifyState)notify->prop_state;
			_lockState = (SvnLockState)notify->lock_state;
			_revision = notify->revision;
			_commandType = commandType;
		}

	private:
		String^ _fullPath;
		String^ _path;
		String^ _mimeType;
		SvnException^ _exception;

	public:
		/// <summary>The path the notification is about</summary>
		/// <remarks>The <see cref="FullPath" /> property contains the path in normalized format; while <see cref="Path" /> returns the exact path from the subversion api</remarks>
		property String^ Path
		{
			String^ get()
			{
				if (!_path && _notify && _notify->path)
				{
					_pathIsUri = (0 != svn_path_is_url(_notify->path));
					_path = SvnBase::Utf8_PtrToString(_notify->path);

					if (!_pathIsUri && _path)
						_path = _path->Replace('/', System::IO::Path::DirectorySeparatorChar);
				}

				return _path;
			}
		}

		/// <summary>The path the notification is about, translated via <see cref="SvnTools::GetNormalizedFullPath" /></summary>
		/// <remarks>The <see cref="FullPath" /> property contains the path in normalized format; while <see cref="Path" /> returns the exact path from the subversion api</remarks>
		property String^ FullPath
		{
			String^ get()
			{
				if (!_fullPath && Path)
					_fullPath = _pathIsUri ? Path : SvnTools::GetNormalizedFullPath(Path);

				return _fullPath;
			}
		}

		/// <summary>Gets a boolean indicating whether the path is a Uri</summary>
		property bool PathIsUri
		{
			bool get()
			{
				GC::KeepAlive(Path);
				return _pathIsUri;
			}
		}

		/// <summary>Gets the commandtype of the command responsible for calling the notify</summary>
		property SvnCommandType CommandType
		{
			SvnCommandType get()
			{
				return _commandType;
			}
		}

		property SvnNotifyAction Action
		{
			SvnNotifyAction get()
			{
				return _action;
			}
		}

		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return _nodeKind;
			}
		}

		property String^ MimeType
		{
			String^ get()
			{
				if (!_mimeType && _notify && _notify->mime_type)
				{
					_mimeType = SvnBase::Utf8_PtrToString(_notify->mime_type);
					_mimeTypeIsBinary = (0 != svn_mime_type_is_binary(_notify->mime_type));
				}

				return _mimeType;
			}
		}

		property bool MimeTypeIsBinary
		{
			bool get()
			{
				GC::KeepAlive(MimeType);
				return _mimeTypeIsBinary;
			}
		}

		property SvnException^ Error
		{
			SvnException^ get();
		}

		property SvnNotifyState ContentState
		{
			SvnNotifyState get()
			{
				return _contentState;
			}
		}

		property SvnNotifyState PropertyState
		{
			SvnNotifyState get()
			{
				return _propertyState;
			}
		}

		property SvnLockState LockState
		{
			SvnLockState get()
			{
				return _lockState;
			}
		}

		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}

		property SvnLockInfo^ Lock
		{
			SvnLockInfo^ get()
			{
				if (!_lock && _notify && _notify->lock)
					_lock = gcnew SvnLockInfo(_notify->lock, false);

				return _lock;
			}
		}

		property String^ ChangeListName
		{
			String^ get()
			{
				if (!_changelistName && _notify && _notify->changelist_name)
					_changelistName = SvnBase::Utf8_PtrToString(_notify->changelist_name);

				return _changelistName;
			}
		}

		property SvnMergeRange^ MergeRange
		{
			SvnMergeRange^ get();
		}

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return SafeGetHashCode(Path) ^ Revision.GetHashCode() ^ ContentState.GetHashCode();
		}

	protected public:
		/// <summary>Detaches the SvnEventArgs from the unmanaged storage; optionally keeping the property values for later use</summary>
		/// <description>After this method is called all properties are either stored managed, or are no longer readable</description>
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					// Use all properties to get them cached in .Net memory
					GC::KeepAlive(Path);
					GC::KeepAlive(MimeType);
					GC::KeepAlive(Error);
					GC::KeepAlive(Lock);
					GC::KeepAlive(ChangeListName);
					GC::KeepAlive(MergeRange);
				}

				if (_lock)
					_lock->Detach(keepProperties);
			}
			finally
			{
				_notify = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};

}