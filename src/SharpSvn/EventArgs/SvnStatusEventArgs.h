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

#include "SvnLockInfo.h"
#include "SvnConflictData.h"
// Included from SvnClientArgs.h

namespace SharpSvn {
	using namespace System::Collections::ObjectModel;
	ref class SvnStatusEventArgs;

	public ref class SvnWorkingCopyInfo sealed
	{
		// This class looks remarkibly simalar to SvnWorkingCopyEntryEventArgs
		// I don't use them as the same to keep both open for future extensions
		// in different directions
		SvnClientContext^ _client;
		const svn_client_status_t *_status;
		const svn_wc_entry_t* _entry;
		AprPool^ _pool;
		bool _ensured;

		String^ _name;
		__int64 _revision;
		Uri^ _uri;
		Uri^ _repositoryUri;
		String^ _repositoryId;
		SvnNodeKind _nodeKind;
		SvnSchedule _schedule;
		bool _copied;
		bool _deleted;
		bool _absent;
		bool _incomplete;
		Uri^ _copyFrom;
		svn_revnum_t _copyFromRev;

		String^ _conflictOld;
		String^ _conflictNew;
		String^ _conflictWork;
		String^ _prejfile;
		DateTime _textTime;
		String^ _checksum;
		svn_revnum_t _lastChangeRev;
		DateTime _lastChangeTime;
		String^ _lastChangeAuthor;
		String^ _lockToken;
		String^ _lockOwner;
		String^ _lockComment;
		DateTime _lockTime;
		bool _hasProperties;
		bool _hasPropertyChanges;
		String^ _changelist;
		__int64 _wcSize;
		bool _keepLocal;
		SvnDepth _depth;
		String^ _fileExternalPath;
		SvnRevision^ _fileExternalRevision;
		SvnRevision^ _fileExternalPegRevision;

	internal:
		SvnWorkingCopyInfo(const svn_client_status_t *status, SvnClientContext^ client, AprPool^ pool);
		void Ensure();

	public:
		/// <summary>The entries name</summary>
		[Obsolete("Use information from SvnStatusEventArgs.Path to avoid expensive lookup")]
		property String^ Name
		{
			String^ get()
			{
				Ensure();
				if (!_name && _entry && _entry->name && _pool)
					_name = SvnBase::Utf8_PathPtrToString(_entry->name, _pool);

				return _name;
			}
		}

		/// <summary>Base revision</summary>
		[Obsolete("Use SvnStatusEventArgs.Revision to avoid expensive lookup")]
		property __int64 Revision
		{
			__int64 get()
			{
				Ensure();
				return _revision;
			}
		}

		/// <summary>Url in repository, including final '/' if the entry specifies a directory</summary>
		[Obsolete("Use SvnStatusEventArgs.Uri to avoid expensive lookup")]
		property System::Uri^ Uri
		{
			System::Uri^ get()
			{
				Ensure();
				if (!_uri && _entry && _entry->url)
					_uri = SvnBase::Utf8_PtrToUri(_entry->url, _nodeKind);

				return _uri;
			}
		}

		/// <summary>The repository Uri including a final '/'</summary>
		[Obsolete("Use SvnStatusEventArgs.RepositoryRoot to avoid expensive lookup")]
		property System::Uri^ RepositoryUri
		{
			System::Uri^ get()
			{
				Ensure();
				if (!_repositoryUri && _entry && _entry->repos)
					_repositoryUri = SvnBase::Utf8_PtrToUri(_entry->repos, SvnNodeKind::Directory);

				return _repositoryUri;
			}
		}

		/// <summary>Gets the repository id as Guid</summary>
		[Obsolete("Use SvnStatusEventArgs.RepositoryId to avoid expensive lookup")]
		property Guid RepositoryId
		{
			Guid get()
			{
				Ensure();
				return RepositoryIdValue ? Guid(RepositoryIdValue) : Guid::Empty;
			}
		}

		/// <summary>Gets the repository id as String</summary>
		[Obsolete("Use SvnStatusEventArgs.RepositoryIdValue to avoid expensive lookup")]
		property String^ RepositoryIdValue
		{
			String^ get()
			{
				Ensure();
				if (!_repositoryId && _entry && _entry->uuid)
					_repositoryId = SvnBase::Utf8_PtrToString(_entry->uuid);

				return _repositoryId;
			}
		}

		/// <summary>Gets the node kind</summary>
		[Obsolete("Use SvnStatusEventArgs.NodeKind to avoid expensive lookup")]
		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				Ensure();
				return _nodeKind;
			}
		}

		/// <summary>Gets the node scheduling (add, delete, replace)</summary>
		[Obsolete("Use SvnStatusEventArgs.NodeStatus to avoid expensive lookup")]
		property SvnSchedule Schedule
		{
			SvnSchedule get()
			{
				Ensure();
				return _schedule;
			}
		}

		/// <summary>Gets a boolean indicating whether the node is in a copied state
		/// (possibly because the entry is a child of a path that is
		/// scheduled for addition or replacement when the entry itself is
		/// normal</summary>
		property bool IsCopy
		{
			bool get()
			{
				Ensure();
				return _copied;
			}
		}

		property bool IsDeleted
		{
			bool get()
			{
				Ensure();
				return _deleted;
			}
		}

		property bool IsAbsent
		{
			bool get()
			{
				Ensure();
				return _absent;
			}
		}

		property bool IsIncomplete
		{
			bool get()
			{
				Ensure();
				return _incomplete;
			}
		}

		property System::Uri^ CopiedFrom
		{
			System::Uri^ get()
			{
				Ensure();
				if (!_copyFrom && _entry && _entry->copyfrom_url)
					_copyFrom = SvnBase::Utf8_PtrToUri(_entry->copyfrom_url, _nodeKind);

				return _copyFrom;
			}
		}

		property __int64 CopiedFromRevision
		{
			__int64 get()
			{
				Ensure();
				return _copyFromRev;
			}
		}

		property String^ ConflictOldFile
		{
			String^ get()
			{
				Ensure();
				if (!_conflictOld && _entry && _entry->conflict_old)
					_conflictOld = SvnBase::Utf8_PtrToString(_entry->conflict_old)->Replace('/', System::IO::Path::DirectorySeparatorChar);

				return _conflictOld;
			}
		}

		property String^ ConflictNewFile
		{
			String^ get()
			{
				Ensure();
				if (!_conflictNew && _entry && _entry->conflict_new)
					_conflictNew = SvnBase::Utf8_PtrToString(_entry->conflict_new)->Replace('/', System::IO::Path::DirectorySeparatorChar);

				return _conflictNew;
			}
		}

		property String^ ConflictWorkFile
		{
			String^ get()
			{
				Ensure();
				if (!_conflictWork && _entry && _entry->conflict_wrk)
					_conflictWork = SvnBase::Utf8_PtrToString(_entry->conflict_wrk)->Replace('/', System::IO::Path::DirectorySeparatorChar);

				return _conflictWork;
			}
		}

		property String^ PropertyRejectFile
		{
			String^ get()
			{
				Ensure();
				if (!_prejfile && _entry && _entry->prejfile)
					_prejfile = SvnBase::Utf8_PtrToString(_entry->prejfile)->Replace('/', System::IO::Path::DirectorySeparatorChar);

				return _prejfile;
			}
		}

		[Obsolete("Not used since Subversion 1.4. Always DateTime.MinValue")]
		property DateTime PropertyChangeTime
		{
			DateTime get() { return DateTime::MinValue; }
		}

		property DateTime ContentChangeTime
		{
			DateTime get()
			{
				Ensure();
				return _textTime;
			}
		}

		property String^ Checksum
		{
			String^ get()
			{
				Ensure();
				if (!_checksum && _entry && _entry->checksum)
					_checksum = SvnBase::Utf8_PtrToString(_entry->checksum);

				return _checksum;
			}
		}

		[Obsolete("Use SvnStatusEventArgs.LastChangeRevision to avoid expensive lookup")]
		property __int64 LastChangeRevision
		{
			__int64 get()
			{
				Ensure();
				return _lastChangeRev;
			}
		}

		[Obsolete("Use SvnStatusEventArgs.LastChangeTime to avoid expensive lookup")]
		property DateTime LastChangeTime
		{
			DateTime get()
			{
				Ensure();
				return _lastChangeTime;
			}
		}

		[Obsolete("Use SvnStatusEventArgs.LastChangeAuthor to avoid expensive lookup")]
		property String^ LastChangeAuthor
		{
			String^ get()
			{
				Ensure();
				if (!_lastChangeAuthor && _entry && _entry->cmt_author)
					_lastChangeAuthor = SvnBase::Utf8_PtrToString(_entry->cmt_author);

				return _lastChangeAuthor;
			}
		}

		[Obsolete("Use SvnStatusEventArgs.LocalLock to avoid expensive lookup")]
		property String^ LockToken
		{
			String^ get()
			{
				Ensure();
				if (!_lockToken && _entry && _entry->lock_token)
					_lockToken = SvnBase::Utf8_PtrToString(_entry->lock_token);

				return _lockToken;
			}
		}

		[Obsolete("Use SvnStatusEventArgs.LocalLock to avoid expensive lookup")]
		property String^ LockOwner
		{
			String^ get()
			{
				Ensure();
				if (!_lockOwner && _entry && _entry->lock_owner)
					_lockOwner = SvnBase::Utf8_PtrToString(_entry->lock_owner);

				return _lockOwner;
			}
		}

		[Obsolete("Use SvnStatusEventArgs.LocalLock to avoid expensive lookup")]
		property String^ LockComment
		{
			String^ get()
			{
				Ensure();
				if (!_lockComment && _entry && _entry->lock_comment)
                {
					_lockComment = SvnBase::Utf8_PtrToString(_entry->lock_comment);

                    if (_lockComment)
                        _lockComment = _lockComment->Replace("\n", Environment::NewLine);
                }

				return _lockComment;
			}
		}

		[Obsolete("Use SvnStatusEventArgs.LocalLock to avoid expensive lookup")]
		property DateTime LockTime
		{
			DateTime get()
			{
				Ensure();
				return _lockTime;
			}
		}

		[Obsolete("Use SvnStatusEventArgs.LocalPropertyStatus to avoid expensive lookup")]
		property bool HasProperties
		{
			bool get()
			{
				Ensure();
				return _hasProperties;
			}
		}

		[Obsolete("Use SvnStatusEventArgs.LocalPropertyStatus to avoid expensive lookup")]
		property bool HasPropertyChanges
		{
			bool get()
			{
				Ensure();
				return _hasPropertyChanges;
			}
		}

		[Obsolete("Use SvnStatusEventArgs.ChangeList to avoid expensive lookup")]
		property String^ ChangeList
		{
			String^ get()
			{
				Ensure();
				if (!_changelist && _entry && _entry->changelist)
					_changelist = SvnBase::Utf8_PtrToString(_entry->changelist);

				return _changelist;
			}
		}

		property __int64 WorkingCopySize
		{
			__int64 get()
			{
				Ensure();
				return _wcSize;
			}
		}

		property bool KeepLocal
		{
			bool get()
			{
				Ensure();
				return _keepLocal;
			}
		}

		[Obsolete("Use SvnStatusEventArgs.Depth to avoid expensive lookup")]
		property SvnDepth Depth
		{
			SvnDepth get()
			{
				Ensure();
				return _depth;
			}
		}

		/// <summary>The entry is a intra-repository file external and this is the
		/// repository root relative path to the file specified in the
		/// externals definition, otherwise NULL if the entry is not a file
		/// external.</summary>
		property String^ FileExternalPath
		{
			String^ get()
			{
				Ensure();
				if (!_fileExternalPath && _entry && _entry->file_external_path)
					_fileExternalPath = SvnBase::Utf8_PtrToString(_entry->file_external_path);

				return _fileExternalPath;
			}
		}

		property SvnRevision^ FileExternalRevision
		{
			SvnRevision^ get()
			{
				Ensure();
				if (!_fileExternalRevision && _entry && _entry->file_external_path)
					_fileExternalRevision = SvnRevision::Load(&_entry->file_external_rev);

				return _fileExternalRevision;
			}
		}

		property SvnRevision^ FileExternalOperationalRevision
		{
			SvnRevision^ get()
			{
				Ensure();
				if (!_fileExternalPegRevision && _entry && _entry->file_external_path)
					_fileExternalPegRevision = SvnRevision::Load(&_entry->file_external_peg_rev);

				return _fileExternalPegRevision;
			}
		}

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return SvnEventArgs::SafeGetHashCode(Name) ^ SvnEventArgs::SafeGetHashCode(Uri);
		}

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
					GC::KeepAlive(ChangeList);
					GC::KeepAlive(FileExternalPath);
					GC::KeepAlive(FileExternalRevision);
					GC::KeepAlive(FileExternalOperationalRevision);
				}
			}
			finally
			{
				_client = nullptr;
				_status = nullptr;
				_entry = nullptr;
				_pool = nullptr;
			}
		}
	};

	public ref class SvnStatusEventArgs : public SvnCancelEventArgs
	{
		const char *_pPath;
		const svn_client_status_t *_status;
		AprPool^ _pool;
		SvnClientContext^ _client;

		String^ _path;
		String^ _fullPath;
		initonly bool _versioned;
		initonly bool _conflicted;
		initonly SvnStatus _wcNodeStatus;
		initonly SvnStatus _wcTextStatus;
		initonly SvnStatus _wcPropertyStatus;
		initonly bool _wcIsLocked;
		initonly bool _copied;
		initonly svn_revnum_t _revision;
		initonly svn_revnum_t _lastChangeRev;
		initonly DateTime _lastChangeTime;
		String^ _lastChangeAuthor;
		Uri^ _reposRoot;
		Uri^ _uri;
        String^ _repositoryId;
		initonly bool _switched;
		initonly bool _fileExternal;
		SvnLockInfo^ _localLock;
		String^ _changelist;
		initonly SvnDepth _depth;

		initonly SvnStatus _reposNodeStatus;
		initonly SvnStatus _reposTextStatus;
		initonly SvnStatus _reposPropertyStatus;
		SvnLockInfo^ _reposLock;
		SvnWorkingCopyInfo^ _wcInfo;

		initonly svn_revnum_t _oodLastCommitRev;
		initonly DateTime _oodLastCommitDate;
		initonly SvnNodeKind _oodLastCommitNodeKind;
		String^ _oodLastCommitAuthor;
		initonly SvnNodeKind _nodeKind;

	internal:
		SvnStatusEventArgs(const char *path, const svn_client_status_t *status, SvnClientContext^ client, AprPool^ pool)
		{
			if (!path)
				throw gcnew ArgumentNullException("path");
			else if (!status)
				throw gcnew ArgumentNullException("status");
			else if (!pool)
				throw gcnew ArgumentNullException("pool");

			_pPath = path;
			_status = status;
			_pool = pool;
			_client = client;

			_versioned = status->versioned != 0;
			_conflicted = status->conflicted != 0;
			_nodeKind = (SvnNodeKind)status->kind;

			_wcNodeStatus = (SvnStatus)status->node_status;
			_wcTextStatus = (SvnStatus)status->text_status;
			_wcPropertyStatus = (SvnStatus)status->prop_status;

			_wcIsLocked = status->wc_is_locked != 0;
			_copied = status->copied != 0;
			_revision = status->revision;

			_lastChangeRev = status->changed_rev;
			_lastChangeTime = SvnBase::DateTimeFromAprTime(status->changed_date);

			_switched = status->switched != 0;
			_fileExternal = status->file_external != 0;
			_depth = (SvnDepth)status->depth;

			_reposNodeStatus = (SvnStatus)status->repos_node_status;
			_reposTextStatus = (SvnStatus)status->repos_text_status;
			_reposPropertyStatus = (SvnStatus)status->repos_prop_status;

			 _oodLastCommitRev = status->ood_changed_rev;
			if (status->ood_changed_rev != SVN_INVALID_REVNUM)
			{
				_oodLastCommitDate = SvnBase::DateTimeFromAprTime(status->ood_changed_date);
				_oodLastCommitNodeKind = (SvnNodeKind)status->ood_kind;
			}

						
		}

	public:
		/// <summary>Gets the recorded node type of this node</summary>
		property SvnNodeKind NodeKind
		{
			[System::Diagnostics::DebuggerStepThrough]
			SvnNodeKind get()
			{
				return _nodeKind;
			}
		}

		/// <summary>The full path the notification is about, as translated via <see cref="SvnTools::GetNormalizedFullPath" /></summary>
		/// <remarks>See also <see cref="Path" />.</remarks>
		property String^ FullPath
		{
			[System::Diagnostics::DebuggerStepThrough]
			String^ get()
			{
				if (!_fullPath && _status && _pool)
					_fullPath = SvnBase::Utf8_PathPtrToString(_status->local_abspath, _pool);

				return _fullPath;
			}
		}

		/// <summary>The path returned by the subversion api</summary>
		property String^ Path
		{
			[System::Diagnostics::DebuggerStepThrough]
			String^ get()
			{
				if (!_path && _pPath && _pool)
					_path = SvnBase::Utf8_PathPtrToString(_pPath, _pool);

				return _path;
			}
		}

		property bool Versioned
		{
			[System::Diagnostics::DebuggerStepThrough]
			bool get()
			{
				return _versioned;
			}
		}

		property bool Conflicted
		{
			[System::Diagnostics::DebuggerStepThrough]
			bool get()
			{
				return _conflicted;
			}
		}

		property bool Modified
		{
			[System::Diagnostics::DebuggerStepThrough]
			bool get()
			{
				switch (_wcNodeStatus)
				{
					case SvnStatus::Modified:
					case SvnStatus::Added:
					case SvnStatus::Deleted:
					case SvnStatus::Replaced:
					case SvnStatus::Merged:
					case SvnStatus::Conflicted:
						return true;
					default:
						return false;
				}
			}
		}

		/// <summary>The node status (combination of restructuring operations, text and property status.</summary>
		property SvnStatus LocalNodeStatus
		{
			[System::Diagnostics::DebuggerStepThrough]
			SvnStatus get()
			{
				return _wcNodeStatus;
			}
		}

		/// <summary>Content status in working copy</summary>
		property SvnStatus LocalContentStatus
		{
			[System::Diagnostics::DebuggerStepThrough]
			SvnStatus get()
			{
				if (LocalNodeStatus != SvnStatus::Modified && LocalNodeStatus != SvnStatus::Conflicted)
				    return LocalNodeStatus;
				else
					return LocalTextStatus;
			}
		}

		/// <summary>The status of the text/content of the node</summary>
		property SvnStatus LocalTextStatus
		{
			[System::Diagnostics::DebuggerStepThrough]
			SvnStatus get()
			{
				return _wcTextStatus;
			}
		}

		/// <summary>Property status in working copy</summary>
		property SvnStatus LocalPropertyStatus
		{
			[System::Diagnostics::DebuggerStepThrough]
			SvnStatus get()
			{
				return _wcPropertyStatus;
			}
		}

		/// <summary>Gets a boolean indicating whether the workingcopy is locked</summary>
		[Obsolete("Please use .Wedged")]
		property bool LocalLocked
		{
			[System::Diagnostics::DebuggerStepThrough]
			bool get() { return Wedged; }
		}

		/// <summary>Gets a boolean indicating whether the workingcopy is locked</summary>
		property bool Wedged
		{
			[System::Diagnostics::DebuggerStepThrough]
			bool get()
			{
				return _wcIsLocked;
			}
		}

		/// <summary>Gets a boolean indicating whether the file is copied in the working copy</summary>
		/// <remarks>A file or directory can be 'copied' if it's scheduled for addition-with-history
		/// (or part of a subtree that is scheduled as such.).</remarks>
		property bool LocalCopied
		{
			[System::Diagnostics::DebuggerStepThrough]
			bool get()
			{
				return _copied;
			}
		}

		property __int64 Revision
		{
			[System::Diagnostics::DebuggerStepThrough]
			__int64 get()
			{
				return _revision;
			}
		}

		property __int64 LastChangeRevision
		{
			[System::Diagnostics::DebuggerStepThrough]
			__int64 get()
			{
				return _lastChangeRev;
			}
		}

		property DateTime LastChangeTime
		{
			[System::Diagnostics::DebuggerStepThrough]
			DateTime get()
			{
				return _lastChangeTime;
			}
		}

		property String^ LastChangeAuthor
		{
			[System::Diagnostics::DebuggerStepThrough]
			String^ get()
			{
				if (!_lastChangeAuthor && _status && _status->changed_author)
					_lastChangeAuthor = SvnBase::Utf8_PtrToString(_status->changed_author);

				return _lastChangeAuthor;
			}
		}

		property Uri^ RepositoryRoot
		{
			[System::Diagnostics::DebuggerStepThrough]
			System::Uri^ get()
			{
				if (!_reposRoot && _status && _status->repos_root_url)
					_reposRoot = SvnBase::Utf8_PtrToUri(_status->repos_root_url, SvnNodeKind::Directory);

				return _reposRoot;
			}
		}

		/// <summary>Gets the repository id as Guid</summary>
		property Guid RepositoryId
		{
			Guid get()
			{
				return RepositoryIdValue ? Guid(RepositoryIdValue) : Guid::Empty;
			}
		}

		/// <summary>Gets the repository id as String</summary>
		property String^ RepositoryIdValue
		{
			String^ get()
			{
				if (!_repositoryId && _status && _status->repos_uuid)
					_repositoryId = SvnBase::Utf8_PtrToString(_status->repos_uuid);

				return _repositoryId;
			}
		}

		property Uri^ Uri
		{
			[System::Diagnostics::DebuggerStepThrough]
			System::Uri^ get()
			{
				if (!_uri && _status && _status->repos_relpath && _pool)
					_uri = SvnBase::Utf8_PtrToUri(svn_path_url_add_component2(_status->repos_root_url, _status->repos_relpath, _pool->Handle),
												  _nodeKind);

				return _uri;
			}
		}

		/// <summary>Gets a boolean indicating whether the file is switched in the working copy</summary>
		property bool Switched
		{
			[System::Diagnostics::DebuggerStepThrough]
			bool get()
			{
				return _switched;
			}
		}

		/// <summary>Gets a boolean indicating whether the node is a file external</summary>
		property bool IsFileExternal
		{
			[System::Diagnostics::DebuggerStepThrough]
			bool get() { return _fileExternal; }
		}

		property SvnLockInfo^ LocalLock
		{
			[System::Diagnostics::DebuggerStepThrough]
			SvnLockInfo^ get()
			{
				if (!_localLock && _status && _status->lock)
					_localLock = gcnew SvnLockInfo(_status->lock, false);

				return _localLock;
			}
		}

		property String^ ChangeList
		{
			[System::Diagnostics::DebuggerStepThrough]
			String^ get()
			{
				if (!_changelist && _status && _status->changelist)
					_changelist = SvnBase::Utf8_PtrToString(_status->changelist);

				return _changelist;
			}
		}

		property SvnDepth Depth
		{
			[System::Diagnostics::DebuggerStepThrough]
			SvnDepth get()
			{
				return _depth;
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

		property SvnStatus RemoteContentStatus
		{
			[System::Diagnostics::DebuggerStepThrough]
			SvnStatus get()
			{
				if (LocalNodeStatus != SvnStatus::Modified && LocalNodeStatus != SvnStatus::Conflicted)
					return RemoteNodeStatus;
				else
					return RemoteTextStatus;
			}
		}

		property SvnStatus RemoteNodeStatus
		{
			[System::Diagnostics::DebuggerStepThrough]
			SvnStatus get()
			{
				return _reposNodeStatus;
			}
		}

		property SvnStatus RemoteTextStatus
		{
			[System::Diagnostics::DebuggerStepThrough]
			SvnStatus get()
			{
				return _reposTextStatus;
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
				if (!_oodLastCommitAuthor && _status && _status->ood_changed_author && IsRemoteUpdated)
					_oodLastCommitAuthor = SvnBase::Utf8_PtrToString(_status->ood_changed_author);

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

		/// <summary>Gets an object containing detailed workingcopy information of this node</summary>
		property SvnWorkingCopyInfo^ WorkingCopyInfo
		{
			[System::Diagnostics::DebuggerStepThrough]
			SvnWorkingCopyInfo^ get()
			{
				if (!_wcInfo && _status && _status->versioned && _client && _pool)
					_wcInfo = gcnew SvnWorkingCopyInfo(_status, _client, _pool);

				return _wcInfo;
			}
		}

		/// <summary>Gets the tree conflict data of this node or <c>null</c> if this node doesn't have a tree conflict</summary>
		[Obsolete("Always returns NULL now; use .IsConflicted and a separate call to SvnClient.Info() to retrieve details")]
		property SvnConflictData^ TreeConflict
		{
			[System::Diagnostics::DebuggerStepThrough]
			SvnConflictData^ get()
			{
				return nullptr;
			}
		}

		

		/// <summary>Gets the raw content status of the node when available</summary>
		[Obsolete("Use .LocalTextStatus")]
		property SvnStatus PristineContentStatus
		{
			[System::Diagnostics::DebuggerStepThrough]
			SvnStatus get() { return LocalTextStatus; }
		}

		/// <summary>Gets the raw property status of the node when available</summary>
		[Obsolete("Use .LocalPropertyStatus")]
		property SvnStatus PristinePropertyStatus
		{
			[System::Diagnostics::DebuggerStepThrough]
			SvnStatus get() { return LocalPropertyStatus; }
		}

	protected public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
					// Use all properties to get them cached in .Net memory
					GC::KeepAlive(FullPath);
					GC::KeepAlive(Path);
					GC::KeepAlive(LastChangeAuthor);
					GC::KeepAlive(RepositoryRoot);
					GC::KeepAlive(RepositoryIdValue);
					GC::KeepAlive(Uri);
					GC::KeepAlive(LocalLock);
					GC::KeepAlive(ChangeList);

					GC::KeepAlive(RemoteLock);
					GC::KeepAlive(RemoteUpdateCommitAuthor);
					GC::KeepAlive(WorkingCopyInfo);
				}

				if (_localLock)
					_localLock->Detach(keepProperties);
				if (_reposLock)
					_reposLock->Detach(keepProperties);
				if (_wcInfo)
					_wcInfo->Detach(keepProperties);
			}
			finally
			{
				_pPath = nullptr;
				_status = nullptr;
				_pool = nullptr;
				_client = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};
}
