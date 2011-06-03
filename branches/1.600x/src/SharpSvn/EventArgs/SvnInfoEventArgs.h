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

#include "SvnConflictData.h"

namespace SharpSvn {

	public ref class SvnInfoEventArgs : public SvnCancelEventArgs
	{
		const svn_info_t* _info;
		AprPool^ _pool;

		initonly String^ _path;
		String^ _fullPath;
		Uri^ _uri;
		initonly __int64 _rev;
		initonly SvnNodeKind _nodeKind;
		Uri^ _reposRootUri;
		String^ _reposUuid;
		initonly __int64 _lastChangeRev;
		initonly DateTime _lastChangeDate;
		String^ _lastChangeAuthor;
		SvnLockInfo^ _lock;
		initonly bool _hasWcInfo;
		SvnSchedule _wcSchedule;
		Uri ^_copyFromUri;
		initonly __int64 _copyFromRev;
		initonly DateTime _contentTime;
		initonly DateTime _propertyTime;
		String^ _checksum;
		String^ _conflict_old;
		String^ _conflict_new;
		String^ _conflict_wrk;
		String^ _prejfile;
		String^ _changelist;
		initonly SvnDepth _depth;
		initonly __int64 _wcSize;
		initonly __int64 _size;
		SvnConflictData^ _treeConflict;

	internal:
		SvnInfoEventArgs(String^ path, const svn_info_t* info, AprPool^ pool)
		{
			if (String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");
			else if (!info)
				throw gcnew ArgumentNullException("info");
			else if (!pool)
				throw gcnew ArgumentNullException("pool");

			_info = info;
			_pool = pool;

			_path = path;
			_rev = info->rev;
			_nodeKind = (SvnNodeKind)info->kind;
			_lastChangeRev = info->last_changed_rev;
			_lastChangeDate = SvnBase::DateTimeFromAprTime(info->last_changed_date);
			_hasWcInfo = info->has_wc_info != 0;
			_wcSchedule = (SvnSchedule)info->schedule;
			_copyFromRev = info->copyfrom_rev;
			if (_hasWcInfo)
			{
				_contentTime = SvnBase::DateTimeFromAprTime(info->text_time);
				_propertyTime = SvnBase::DateTimeFromAprTime(info->prop_time);
			}

			_depth = (SvnDepth)info->depth;

			if (info->size64 == SVN_INVALID_FILESIZE)
				_size = -1;
			else
				_size = info->size64;

			if (info->working_size64 == SVN_INVALID_FILESIZE)
				_wcSize = -1;
			else
				_wcSize = info->working_size64;
		}
	public:
		/// <summary>Gets the path of the file. The local path if requisting WORKING version, otherwise the name of the file
		/// at the specified version</summary>
		property String^ Path
		{
			String^ get()
			{
				return _path;
			}
		}

		/// <summary>The path the notification is about, translated via <see cref="SvnTools::GetNormalizedFullPath" /></summary>
		/// <remarks>The <see cref="FullPath" /> property contains the path in normalized format; while <see cref="Path" /> returns the exact path from the subversion api</remarks>
		property String^ FullPath
		{
			String^ get()
			{
				if (!_fullPath && Path && HasLocalInfo)
					_fullPath = SvnTools::GetNormalizedFullPath(Path);

				return _fullPath;
			}
		}

		/// <summary>Gets the full Uri of the node</summary>
		property Uri^ Uri
		{
			System::Uri^ get()
			{
				if (!_uri && _info && _info->URL)
					_uri = SvnBase::Utf8_PtrToUri(_info->URL, _nodeKind);

				return _uri;
			}
		}

		/// <summary>Gets the queried revision</summary>
		property __int64 Revision
		{
			__int64 get()
			{
				return _rev;
			}
		}

		/// <summary>Gets the node kind of the specified node</summary>
		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return _nodeKind;
			}
		}

		/// <summary>Gets the repository root Uri; ending in a '/'</summary>
		/// <remarks>The unmanaged api does not add the '/' at the end, but this makes using <see cref="System::Uri" /> hard</remarks>
		property System::Uri^ RepositoryRoot
		{
			System::Uri^ get()
			{
				if (!_reposRootUri && _info && _info->repos_root_URL)
					_reposRootUri = SvnBase::Utf8_PtrToUri(_info->repos_root_URL, SvnNodeKind::Directory);

				return _reposRootUri;
			}
		}

		/// <summary>Gets the uuid of the repository (if available). Otherwise Guid.Empty</summary>
		property Guid RepositoryId
		{
			Guid get()
			{
				return RepositoryIdValue ? Guid(RepositoryIdValue) : Guid::Empty;
			}
		}

		/// <summary>Gets the repository uuid or <c>null</c> if not available</summary>
		property String^ RepositoryIdValue
		{
			String^ get()
			{
				if (!_reposUuid && _info && _info->repos_UUID)
					_reposUuid = SvnBase::Utf8_PtrToString(_info->repos_UUID);

				return _reposUuid;
			}
		}

		/// <summary>Gets the last revision in which node (or one of its descendants) changed</summary>
		property __int64 LastChangeRevision
		{
			__int64 get()
			{
				return _lastChangeRev;
			}
		}

		/// <summary>Gets the timestamp of the last revision in which node (or one of its descendants) changed</summary>
		property DateTime LastChangeTime
		{
			DateTime get()
			{
				return _lastChangeDate;
			}
		}

		/// <summary>Gets the author of the last revision in which node (or one of its descendants) changed</summary>
		property String^ LastChangeAuthor
		{
			String^ get()
			{
				if (!_lastChangeAuthor && _info && _info->last_changed_author)
					_lastChangeAuthor = SvnBase::Utf8_PtrToString(_info->last_changed_author);

				return _lastChangeAuthor;
			}
		}

		/// <summary>Gets information about the current lock on node</summary>
		property SvnLockInfo^ Lock
		{
			SvnLockInfo^ get()
			{
				if (!_lock && _info && _info->lock)
					_lock = gcnew SvnLockInfo(_info->lock, HasLocalInfo);

				return _lock;
			}
		}

		/// <summary>Gets a boolean indicating whether working copy information is available in this instance</summary>
		property bool HasLocalInfo
		{
			bool get()
			{
				return _hasWcInfo;
			}
		}

		property SvnSchedule Schedule
		{
			SvnSchedule get()
			{
				return HasLocalInfo ? _wcSchedule : SvnSchedule::Normal;
			}
		}

		property System::Uri^ CopyFromUri
		{
			System::Uri^ get()
			{
				if (!_copyFromUri && _info && _info->copyfrom_url && HasLocalInfo)
					_copyFromUri = SvnBase::Utf8_PtrToUri(_info->copyfrom_url, _nodeKind);

				return _copyFromUri;
			}
		}

		property __int64 CopyFromRevision
		{
			__int64 get()
			{
				return _copyFromRev;
			}
		}

		[Obsolete("Please use .CopyFromRevision")]
		property __int64 CopyFromRev
		{
			__int64 get()
			{
				return CopyFromRevision;
			}
		}

		property DateTime ContentTime
		{
			DateTime get()
			{
				return _contentTime;
			}
		}

		property DateTime PropertyTime
		{
			DateTime get()
			{
				return _propertyTime;
			}
		}

		property String^ Checksum
		{
			String^ get()
			{
				if (!_checksum && _info && _info->checksum && HasLocalInfo)
					_checksum = SvnBase::Utf8_PtrToString(_info->checksum);

				return _checksum;
			}
		}

		property String^ ConflictOld
		{
			String^ get()
			{
				if (!_conflict_old && _info && _info->conflict_old && HasLocalInfo)
					_conflict_old = SvnBase::Utf8_PtrToString(_info->conflict_old);

				return _conflict_old;
			}
		}

		property String^ ConflictNew
		{
			String^ get()
			{
				if (!_conflict_new && _info && _info->conflict_new && HasLocalInfo)
					_conflict_new = SvnBase::Utf8_PtrToString(_info->conflict_new);

				return _conflict_new;
			}
		}

		property String^ ConflictWork
		{
			String^ get()
			{
				if (!_conflict_wrk && _info && _info->conflict_wrk && HasLocalInfo)
					_conflict_wrk = SvnBase::Utf8_PtrToString(_info->conflict_wrk);

				return _conflict_wrk;
			}
		}

		property String^ PropertyEditFile
		{
			String^ get()
			{
				if (!_prejfile && _info && _info->prejfile && HasLocalInfo)
					_prejfile = SvnBase::Utf8_PtrToString(_info->prejfile);

				return _prejfile;
			}
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
		}

		property String^ ChangeList
		{
			String^ get()
			{
				if (!_changelist && _info && _info->changelist)
					_changelist = SvnBase::Utf8_PtrToString(_info->changelist);

				return _changelist;
			}
		}

		property __int64 WorkingCopySize
		{
			__int64 get()
			{
				return _wcSize;
			}
		}

		property __int64 RepositorySize
		{
			__int64 get()
			{
				return _size;
			}
		}

		property SvnConflictData^ TreeConflict
		{
			SvnConflictData^ get()
			{
				if (!_treeConflict && _info && _info->tree_conflict && _pool)
					_treeConflict = gcnew SvnConflictData(_info->tree_conflict, _pool);

				return _treeConflict;
			}
		}

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return SafeGetHashCode(Path) ^ SafeGetHashCode(Uri);
		}

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					// Use all properties to get them cached in .Net memory
					GC::KeepAlive(Path);
					GC::KeepAlive(Uri);
					GC::KeepAlive(RepositoryRoot);
					GC::KeepAlive(RepositoryId);
					GC::KeepAlive(LastChangeAuthor);
					GC::KeepAlive(Lock);
					GC::KeepAlive(CopyFromUri);
					GC::KeepAlive(Checksum);
					GC::KeepAlive(ConflictOld);
					GC::KeepAlive(ConflictNew);
					GC::KeepAlive(ConflictWork);
					GC::KeepAlive(PropertyEditFile);
					GC::KeepAlive(ChangeList);
					GC::KeepAlive(TreeConflict);
				}

				if (_lock)
					_lock->Detach(keepProperties);
				if (_treeConflict)
					_treeConflict->Detach(keepProperties);
			}
			finally
			{
				_info = nullptr;
				_pool = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};

}