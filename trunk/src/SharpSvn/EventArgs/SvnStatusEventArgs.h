// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnLockInfo.h"
// Included from SvnClientArgs.h

namespace SharpSvn {
	using namespace System::Collections::ObjectModel;

	public ref class SvnWorkingCopyInfo sealed
	{
		svn_wc_entry_t* _entry;

		String^ _name;
		initonly __int64 _revision;
		Uri^ _uri;
		Uri^ _repositoryUri;
		String^ _repositoryId;
		initonly SvnNodeKind _nodeKind;
		initonly SvnSchedule _schedule;
		initonly bool _copied;
		initonly bool _deleted;
		initonly bool _absent;
		initonly bool _incomplete;
		Uri^ _copyFrom;
		initonly __int64 _copyFromRev;

		String^ _conflictOld;
		String^ _conflictNew;
		String^ _conflictWork;
		String^ _prejfile;
		initonly DateTime _propertyTime;
		initonly DateTime _textTime;
		String^ _checksum;
		initonly __int64 _lastChangeRev;
		initonly DateTime _lastChangeTime;
		String^ _lastChangeAuthor;
		String^ _lockToken;
		String^ _lockOwner;
		String^ _lockComment;
		initonly DateTime _lockTime;
		initonly bool _hasProperties;
		initonly bool _hasPropertyChanges;
		Collection<String^>^ _cachableProperties;
		Collection<String^>^ _availableProperties;
		String^ _changelist;
		initonly __int64 _wcSize;
		initonly bool _keepLocal;
		initonly SvnDepth _depth;

	internal:
		SvnWorkingCopyInfo(svn_wc_entry_t *entry)
		{
			if (!entry)
				throw gcnew ArgumentNullException("entry");

			_entry = entry;

			_revision = entry->revision;
			_nodeKind = (SvnNodeKind)entry->kind;
			_schedule = (SvnSchedule)entry->schedule;
			_copied = (entry->copied != 0);
			_deleted = (entry->deleted != 0);
			_absent = (entry->absent != 0);
			_incomplete = (entry->incomplete != 0);
			_copyFromRev = entry->copyfrom_rev;
			_propertyTime = SvnBase::DateTimeFromAprTime(entry->prop_time);
			_textTime = SvnBase::DateTimeFromAprTime(entry->text_time);
			_lastChangeRev = entry->cmt_rev;
			_lastChangeTime = SvnBase::DateTimeFromAprTime(entry->cmt_date);
			_lockTime = SvnBase::DateTimeFromAprTime(entry->lock_creation_date);
			_hasProperties = (entry->has_props != 0);
			_hasPropertyChanges = (entry->has_prop_mods != 0);
			_wcSize = entry->working_size;
			_keepLocal = (entry->keep_local != 0);
			_depth = (SvnDepth)entry->depth;
		}

	public:
		property String^ Name
		{
			String^ get()
			{
				if (!_name && _entry && _entry->name)
					_name = SvnBase::Utf8_PtrToString(_entry->name)->Replace('/', System::IO::Path::DirectorySeparatorChar);

				return _name;
			}
		}

		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}

		property System::Uri^ Uri
		{
			System::Uri^ get()
			{
				if (!_uri && _entry && _entry->url)
					_uri = SvnBase::Utf8_PtrToUri(_entry->url, _nodeKind);

				return _uri;
			}
		}

		property System::Uri^ RepositoryUri
		{
			System::Uri^ get()
			{
				if (!_repositoryUri && _entry && _entry->repos)
					_repositoryUri = SvnBase::Utf8_PtrToUri(_entry->repos, SvnNodeKind::Directory);

				return _repositoryUri;
			}
		}

		property Guid RepositoryId
		{
			Guid get()
			{
				if (!_repositoryId && _entry && _entry->uuid)
					_repositoryId = SvnBase::Utf8_PtrToString(_entry->uuid);

				return _repositoryId ? Guid(_repositoryId) : Guid::Empty;
			}
		}

		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return _nodeKind;
			}
		}

		property SvnSchedule Schedule
		{
			SvnSchedule get()
			{
				return _schedule;
			}
		}

		property bool IsCopy
		{
			bool get()
			{
				return _copied;
			}
		}

		property bool IsDeleted
		{
			bool get()
			{
				return _deleted;
			}
		}

		property bool IsAbsent
		{
			bool get()
			{
				return _absent;
			}
		}

		property bool IsIncomplete
		{
			bool get()
			{
				return _incomplete;
			}
		}

		property System::Uri^ CopiedFrom
		{
			System::Uri^ get()
			{
				if (!_copyFrom && _entry && _entry->copyfrom_url)
					_copyFrom = SvnBase::Utf8_PtrToUri(_entry->copyfrom_url, _nodeKind);

				return _copyFrom;
			}
		}

		property __int64 CopiedFromRevision
		{
			__int64 get()
			{
				return _copyFromRev;
			}
		}

		property String^ ConflictOldFile
		{
			String^ get()
			{
				if (!_conflictOld && _entry && _entry->conflict_old)
					_conflictOld = SvnBase::Utf8_PtrToString(_entry->conflict_old)->Replace('/', System::IO::Path::DirectorySeparatorChar);

				return _conflictOld;
			}
		}

		property String^ ConflictNewFile
		{
			String^ get()
			{
				if (!_conflictNew && _entry && _entry->conflict_new)
					_conflictNew = SvnBase::Utf8_PtrToString(_entry->conflict_new)->Replace('/', System::IO::Path::DirectorySeparatorChar);

				return _conflictNew;
			}
		}

		property String^ ConflictWorkFile
		{
			String^ get()
			{
				if (!_conflictWork && _entry && _entry->conflict_wrk)
					_conflictWork = SvnBase::Utf8_PtrToString(_entry->conflict_wrk)->Replace('/', System::IO::Path::DirectorySeparatorChar);

				return _conflictWork;
			}
		}

		property String^ PropertyRejectFile
		{
			String^ get()
			{
				if (!_prejfile && _entry && _entry->prejfile)
					_prejfile = SvnBase::Utf8_PtrToString(_entry->prejfile)->Replace('/', System::IO::Path::DirectorySeparatorChar);

				return _prejfile;
			}
		}

		property DateTime PropertyChangeTime
		{
			DateTime get()
			{
				return _propertyTime;
			}
		}

		property DateTime ContentChangeTime
		{
			DateTime get()
			{
				return _textTime;
			}
		}

		property String^ Checksum
		{
			String^ get()
			{
				if (!_checksum && _entry && _entry->checksum)
					_checksum = SvnBase::Utf8_PtrToString(_entry->checksum);

				return _checksum;
			}
		}

		property __int64 LastChangeRevision
		{
			__int64 get()
			{
				return _lastChangeRev;
			}
		}

		property DateTime LastChangeTime
		{
			DateTime get()
			{
				return _lastChangeTime;
			}
		}

		property String^ LastChangeAuthor
		{
			String^ get()
			{
				if (!_lastChangeAuthor && _entry && _entry->cmt_author)
					_lastChangeAuthor = SvnBase::Utf8_PtrToString(_entry->cmt_author);

				return _lastChangeAuthor;
			}
		}

		property String^ LockToken
		{
			String^ get()
			{
				if (!_lockToken && _entry && _entry->lock_token)
					_lockToken = SvnBase::Utf8_PtrToString(_entry->lock_token);

				return _lockToken;
			}
		}

		property String^ LockOwner
		{
			String^ get()
			{
				if (!_lockOwner && _entry && _entry->lock_owner)
					_lockOwner = SvnBase::Utf8_PtrToString(_entry->lock_owner);

				return _lockOwner;
			}
		}

		property String^ LockComment
		{
			String^ get()
			{
				if (!_lockComment && _entry && _entry->lock_comment)
					_lockComment = SvnBase::Utf8_PtrToString(_entry->lock_comment);

				return _lockOwner;
			}
		}

		property DateTime LockTime
		{
			DateTime get()
			{
				return _lockTime;
			}
		}

		property bool HasProperties
		{
			bool get()
			{
				return _hasProperties;
			}
		}

		property bool HasPropertyChanges
		{
			bool get()
			{
				return _hasPropertyChanges;
			}
		}

		property Collection<String^>^ CacheableProperties
		{
			Collection<String^>^ get()
			{
				if (!_cachableProperties && _entry && _entry->cachable_props)
				{
					Collection<String^>^ items = gcnew Collection<String^>();

					for each (String^ i in SvnBase::Utf8_PtrToString(_entry->cachable_props)->Split(' '))
					{
						if (!String::IsNullOrEmpty(i))
							items->Add(i);
					}

					_cachableProperties = items;
				}
				return _cachableProperties;
			}
		}

		property Collection<String^>^ AvailableProperties
		{
			Collection<String^>^ get()
			{
				if (!_availableProperties && _entry && _entry->present_props)
				{
					Collection<String^>^ items = gcnew Collection<String^>();

					for each (String^ i in SvnBase::Utf8_PtrToString(_entry->present_props)->Split(' '))
					{
						if (!String::IsNullOrEmpty(i))
							items->Add(i);
					}

					_availableProperties = items;
				}
				return _availableProperties;
			}
		}

		property String^ ChangeList
		{
			String^ get()
			{
				if (!_changelist && _entry && _entry->changelist)
					_changelist = SvnBase::Utf8_PtrToString(_entry->changelist);

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

		property bool KeepLocal
		{
			bool get()
			{
				return _keepLocal;
			}
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
		}

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return Depth.GetHashCode() ^ SvnEventArgs::SafeGetHashCode(Name) ^ SvnEventArgs::SafeGetHashCode(Uri) ^ Revision.GetHashCode();
		}

	protected public:
		virtual void Detach(bool keepProperties)
		{
			try
			{
				if (keepProperties)
				{
					GC::KeepAlive(Name);
					GC::KeepAlive(Uri);
					GC::KeepAlive(RepositoryUri);
					GC::KeepAlive(RepositoryId);
					GC::KeepAlive(CopiedFrom);
					GC::KeepAlive(ConflictOldFile);
					GC::KeepAlive(ConflictNewFile);
					GC::KeepAlive(ConflictWorkFile);
					GC::KeepAlive(PropertyRejectFile);
					GC::KeepAlive(Checksum);
					GC::KeepAlive(LastChangeAuthor);
					GC::KeepAlive(LockToken);
					GC::KeepAlive(LockOwner);
					GC::KeepAlive(LockComment);
					GC::KeepAlive(CacheableProperties);
					GC::KeepAlive(AvailableProperties);
					GC::KeepAlive(ChangeList);
				}
			}
			finally
			{
				_entry = nullptr;
			}
		}
	};

	public ref class SvnStatusEventArgs : public SvnEventArgs
	{
		initonly String^ _path;
		const svn_wc_status2_t *_status;
		String^ _fullPath;
		initonly SvnStatus _wcContentStatus;
		initonly SvnStatus _wcPropertyStatus;
		initonly bool _wcLocked;
		initonly bool _copied;
		initonly bool _switched;
		initonly SvnStatus _reposContentStatus;
		initonly SvnStatus _reposPropertyStatus;
		SvnLockInfo^ _reposLock;
		SvnWorkingCopyInfo^ _wcInfo;
		Uri^ _uri;
		initonly __int64 _oodLastCommitRev;
		initonly DateTime _oodLastCommitDate;
		initonly SvnNodeKind _oodLastCommitNodeKind;
		String^ _oodLastCommitAuthor;
		initonly SvnNodeKind _nodeKind;

	internal:
		SvnStatusEventArgs(String^ path, const svn_wc_status2_t *status)
		{
			if (String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");
			else if(!status)
				throw gcnew ArgumentNullException("status");

			_path = path->Replace('/', System::IO::Path::DirectorySeparatorChar);
			_status = status;

			_wcContentStatus = (SvnStatus)status->text_status;
			_wcPropertyStatus = (SvnStatus)status->prop_status;
			_wcLocked = status->locked != 0;
			_copied = status->copied != 0;
			_switched = status->switched != 0;
			_reposContentStatus = (SvnStatus)status->repos_text_status;
			_reposPropertyStatus = (SvnStatus)status->repos_prop_status;

			_oodLastCommitRev = status->ood_last_cmt_rev;

			_nodeKind = status->entry ? (SvnNodeKind)status->entry->kind : SvnNodeKind::None;

			if (status->ood_last_cmt_rev != SVN_INVALID_REVNUM)
			{
				_oodLastCommitDate = SvnBase::DateTimeFromAprTime(status->ood_last_cmt_date);
				_oodLastCommitNodeKind = (SvnNodeKind)status->ood_kind;
			}
		}

	public:
		property String^ Path
		{
			String^ get()
			{
				return _path;
			}
		}

		/// <summary>The path the notification is about, translated via <see cref="System::IO::Path::GetFullPath" /></summary>
		/// <remarks>The <see cref="FullPath" /> property contains the path in normalized format; while <see cref="Path" /> returns the exact path from the subversion api</remarks>
		property String^ FullPath
		{
			String^ get()
			{
				if (!_fullPath && Path)
					_fullPath = System::IO::Path::GetFullPath(Path);

				return _fullPath;
			}
		}

		/// <summary>Content status in working copy</summary>
		property SvnStatus LocalContentStatus
		{
			SvnStatus get()
			{
				return _wcContentStatus;
			}
		}

		/// <summary>Property status in working copy</summary>
		property SvnStatus LocalPropertyStatus
		{
			SvnStatus get()
			{
				return _wcPropertyStatus;
			}
		}

		/// <summary>Gets a boolean indicating whether the workingcopy is locked</summary>
		property bool LocalLocked
		{
			bool get()
			{
				return _wcLocked;
			}
		}

		/// <summary>Gets a boolean indicating whether the file is copied in the working copy</summary>
		/// <remarks>A file or directory can be 'copied' if it's scheduled for addition-with-history
		/// (or part of a subtree that is scheduled as such.).</remarks>
		property bool LocalCopied
		{
			bool get()
			{
				return _copied;
			}
		}

		/// <summary>Gets a boolean indicating whether the file is switched in the working copy</summary>
		property bool Switched
		{
			bool get()
			{
				return _switched;
			}
		}

		property SvnStatus RemoteContentStatus
		{
			SvnStatus get()
			{
				return _reposContentStatus;
			}
		}

		property SvnStatus RemotePropertyStatus
		{
			SvnStatus get()
			{
				return _reposPropertyStatus;
			}
		}

		property SvnLockInfo^ RemoteLock
		{
			SvnLockInfo^ get()
			{
				if (!_reposLock && _status && _status->repos_lock)
					_reposLock = gcnew SvnLockInfo(_status->repos_lock, false);

				return _reposLock;
			}
		}

		property Uri^ Uri
		{
			System::Uri^ get()
			{
				if (!_uri && _status && _status->url)
					_uri = SvnBase::Utf8_PtrToUri(_status->url, _nodeKind);

				return _uri;
			}
		}

		/// <summary>Gets the out of date status of the item; if true the RemoteUpdate* properties are set</summary>
		property bool IsRemoteUpdated
		{
			bool get()
			{
				return _oodLastCommitRev != SVN_INVALID_REVNUM;
			}
		}

		/// <summary>Out of Date: Last commit version of the item</summary>
		property __int64 RemoteUpdateRevision
		{
			__int64 get()
			{
				return _oodLastCommitRev;
			}
		}

		/// <summary>Out of Date: Last commit date of the item</summary>
		property DateTime RemoteUpdateCommitTime
		{
			DateTime get()
			{
				return _oodLastCommitDate;
			}
		}

		/// <summary>Out of Date: Gets the author of the OutOfDate commit</summary>
		property String^ RemoteUpdateCommitAuthor
		{
			String^ get()
			{
				if (!_oodLastCommitAuthor && _status && _status->ood_last_cmt_author && IsRemoteUpdated)
					_oodLastCommitAuthor = SvnBase::Utf8_PtrToString(_status->ood_last_cmt_author);

				return _oodLastCommitAuthor;
			}
		}

		/// <summary>Out of Date: Gets the node kind of the OutOfDate commit</summary>
		property SvnNodeKind RemoteUpdateNodeKind
		{
			SvnNodeKind get()
			{
				return _oodLastCommitNodeKind;
			}
		}

		property SvnWorkingCopyInfo^ WorkingCopyInfo
		{
			[System::Diagnostics::DebuggerStepThrough]
			SvnWorkingCopyInfo^ get()
			{
				if (!_wcInfo && _status && _status->entry)
					_wcInfo = gcnew SvnWorkingCopyInfo(_status->entry);

				return _wcInfo;
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
				if (keepProperties)
				{
					// Use all properties to get them cached in .Net memory
					GC::KeepAlive(Uri);
					GC::KeepAlive(RemoteLock);
					GC::KeepAlive(RemoteUpdateCommitAuthor);
					GC::KeepAlive(WorkingCopyInfo);
				}

				if (_reposLock)
					_reposLock->Detach(keepProperties);
				if (_wcInfo)
					_wcInfo->Detach(keepProperties);
			}
			finally
			{
				_status = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};

}