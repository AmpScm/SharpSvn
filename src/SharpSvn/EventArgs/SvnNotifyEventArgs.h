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

// Included from SvnClientArgs.h

#include "SvnLockInfo.h"

namespace SharpSvn {

	public ref class SvnNotifyEventArgs sealed : public SvnEventArgs
	{
		const svn_wc_notify_t *_notify;
		AprPool^ _pool;


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
		String^ _propertyName;
		SvnPropertyCollection^ _revProps;

	internal:
		SvnNotifyEventArgs(const svn_wc_notify_t *notify, SvnCommandType commandType, AprPool^ pool)
		{
			if (!notify)
				throw gcnew ArgumentNullException("notify");
			else if (!pool)
				throw gcnew ArgumentNullException("pool");

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
		System::Uri^ _uri;
		String^ _mimeType;
		SvnException^ _exception;

	public:
		/// <summary>The path the notification is about</summary>
		/// <remarks>The <see cref="FullPath" /> property contains the path in normalized format; while <see cref="Path" /> returns the exact path from the subversion api</remarks>
		property String^ Path
		{
			String^ get()
			{
				if (!_path && !_uri && _notify && _pool && _notify->path && !_notify->url)
				{
					if (svn_path_is_url(_notify->path))
						_uri = SvnBase::Utf8_PtrToUri(_notify->path, SvnNodeKind::None);
					else
						_path = SvnBase::Utf8_PathPtrToString(_notify->path, _pool);
				}

				return _path;
			}
		}

		/// <summary>Gets the (relative or absolute uri) Uri the notification is about</summary>
		property System::Uri^ Uri
		{
			System::Uri^ get()
			{
				if (!_uri && _notify && _notify->url)
					System::Uri::TryCreate(SvnBase::Utf8_PtrToString(_notify->url), UriKind::RelativeOrAbsolute, _uri);

				return _uri;
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
		[Obsolete("Check .Uri and (Path == null)")]
		property bool PathIsUri
		{
			bool get()
			{
				GC::KeepAlive(Path);
				return Uri || _pathIsUri;
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

		/// <summary>Action that describes what happened to path/url</summary>
		property SvnNotifyAction Action
		{
			SvnNotifyAction get()
			{
				return _action;
			}
		}

		/// <summary>Node kind of path/url</summary>
		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return _nodeKind;
			}
		}

		/// <summary>If non-NULL, indicates the mime-type of @c path. It is always @c NULL for directories.</summary>
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

		/// <summary>If MimeType is not null, a boolean indicating whether this mime type is interpreted as binary</summary>
		property bool MimeTypeIsBinary
		{
			bool get()
			{
				GC::KeepAlive(MimeType);
				return _mimeTypeIsBinary;
			}
		}

		/// <summary>Points to an error describing the reason for the failure when
		/// action is one of the following: @c svn_wc_notify_failed_lock, svn_wc_notify_failed_unlock, 
		/// svn_wc_notify_failed_external. Is @c NULL otherwise.</summary>
		property SvnException^ Error
		{
			SvnException^ get();
		}

		/// <summary>The type of notification that is occurring about node content.</summary>
		property SvnNotifyState ContentState
		{
			SvnNotifyState get()
			{
				return _contentState;
			}
		}

		/// <summary>The type of notification that is occurring about node properties.</summary>
		property SvnNotifyState PropertyState
		{
			SvnNotifyState get()
			{
				return _propertyState;
			}
		}

		/// <summary>Reflects the addition or removal of a lock token in the working copy.</summary>
		property SvnLockState LockState
		{
			SvnLockState get()
			{
				return _lockState;
			}
		}

		/// <summary>When action is svn_wc_notify_update_completed, target revision
		/// of the update, or @c SVN_INVALID_REVNUM if not available; when action is 
		/// c svn_wc_notify_blame_revision, processed revision. In all other cases, 
		/// it is @c SVN_INVALID_REVNUM.</summary>
		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}

		/// <summary>Points to the lock structure received from the repository when
		/// action is @c svn_wc_notify_locked.  For other actions, it is
		/// NULL.</summary>
		property SvnLockInfo^ Lock
		{
			SvnLockInfo^ get()
			{
				if (!_lock && _notify && _notify->lock)
					_lock = gcnew SvnLockInfo(_notify->lock, false);

				return _lock;
			}
		}

		/// <summary>When action is svn_wc_notify_changelist_add or name.  In all other
		/// cases, it is NULL</summary>
		property String^ ChangeListName
		{
			String^ get()
			{
				if (!_changelistName && _notify && _notify->changelist_name)
					_changelistName = SvnBase::Utf8_PtrToString(_notify->changelist_name);

				return _changelistName;
			}
		}

		/// <summary>When @c action is @c svn_wc_notify_merge_begin, and both the left and right sides 
		/// of the merge are from the same URL.  In all other cases, it is NULL</summary>
		property SvnMergeRange^ MergeRange
		{
			SvnMergeRange^ get();
		}

		/// <summary>If action relates to properties, specifies the name of the property.</summary>
		property String^ PropertyName
		{
			String^ get()
			{
				if (!_propertyName && _notify && _notify->prop_name)
					_propertyName = SvnBase::Utf8_PtrToString(_notify->prop_name);

				return _propertyName;
			}
		}

		/// <summary>If action is svn_wc_notify_blame_revision, contains a list of revision properties for the specified revision</summary>
		property SvnPropertyCollection^ RevisionProperties
		{
			SvnPropertyCollection^ get()
			{
				if(!_revProps && _notify && _notify->rev_props && _pool)
					_revProps = SvnBase::CreatePropertyDictionary(_notify->rev_props, _pool);

				return _revProps;
			}
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
					GC::KeepAlive(Uri);
					GC::KeepAlive(MimeType);
					GC::KeepAlive(Error);
					GC::KeepAlive(Lock);
					GC::KeepAlive(ChangeListName);
					GC::KeepAlive(MergeRange);
					GC::KeepAlive(PropertyName);
					GC::KeepAlive(RevisionProperties);
				}

				if (_lock)
					_lock->Detach(keepProperties);
			}
			finally
			{
				_notify = nullptr;
				_pool = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};

}