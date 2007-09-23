
#pragma once

#include "SvnEnums.h"
#include "SvnCommitItem.h"

namespace SharpSvn {
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IDictionary;
	using System::Collections::Generic::IList;
	using System::Collections::Generic::SortedList;

	ref class SvnException;

	public interface class ISvnDetachable
	{
		void Detach(bool keepProperties);
	};

	public ref class SvnEventArgs abstract : public System::EventArgs, public ISvnDetachable
	{
	protected:
		SvnEventArgs()
		{
		}

	public:
		/// <summary>Detaches the SvnEventArgs from the unmanaged storage; optionally keeping the property values for later use</summary>
		/// <description>After this method is called all properties are either stored managed, or are no longer readable</description>
		virtual void Detach(bool keepProperties)
		{
			UNUSED_ALWAYS(keepProperties);
		}
	};

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
		IList<SvnCommitItem^>^ _items;
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

		property IList<SvnCommitItem^>^ Items
		{
			IList<SvnCommitItem^>^ get();
		}

		virtual void Detach(bool keepProperties) override;
	};

	public ref class SvnConflictEventArgs sealed : public SvnCancelEventArgs
	{
		const svn_wc_conflict_description_t* _description;
		SvnConflictResult _result;

		initonly SvnNodeKind _nodeKind;
		initonly bool _isBinary;
		initonly SvnConflictReason _reason;
		
		String^ _path;
		String^ _mimeType;
		String^ _baseFile;
		String^ _reposFile;
		String^ _userFile;
		String^ _mergedFile;

	internal:
		SvnConflictEventArgs(const svn_wc_conflict_description_t *description)
		{
			if(!description)
				throw gcnew ArgumentNullException("description");

			_description = description;
			_result = SvnConflictResult::Conflicted;

			_nodeKind = (SvnNodeKind)description->node_kind;
			_reason = (SvnConflictReason)description->reason;
		}

	public:
		property SvnConflictResult Result
		{
			SvnConflictResult get()
			{
				return _result;
			}
			void set(SvnConflictResult value)
			{
				_result = value;
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

		property String^ RemoteFile
		{
			String^ get()
			{
				if(!_reposFile && _description && _description->repos_file)
					_reposFile = SvnBase::Utf8_PtrToString(_description->repos_file);

				return _reposFile;
			}
		}

		property String^ UserFile
		{
			String^ get()
			{
				if(!_userFile && _description && _description->user_file)
					_userFile = SvnBase::Utf8_PtrToString(_description->user_file);

				return _userFile;
			}
		}

		property String^ MergedFile
		{
			String^ get()
			{
				if(!_mergedFile && _description && _description->merged_file)
					_mergedFile  = SvnBase::Utf8_PtrToString(_description->merged_file);

				return _mergedFile;
			}
		}

		property bool IsBinary
		{
			bool get()
			{
				return _isBinary;
			}
		}

		property SvnConflictReason ConflictReason
		{
			SvnConflictReason get()
			{
				return _reason;
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
					GC::KeepAlive(MimeType);
					GC::KeepAlive(BaseFile);
					GC::KeepAlive(RemoteFile);
					GC::KeepAlive(UserFile);
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

	public ref class SvnLockInfo sealed : public ISvnDetachable
	{
		const svn_lock_t *_lock;
		initonly bool _localData;
		String^ _path;
		String^ _fullPath;
		String^ _token;
		String^ _owner;
		String^ _comment;
		initonly bool _isDavComment;
		initonly DateTime _creationDate;
		initonly DateTime _expirationDate;
	internal:
		SvnLockInfo(const svn_lock_t *lock, bool localData)
		{
			if(!lock)
				throw gcnew ArgumentNullException("lock");

			_localData = localData;
			_lock = lock;
			_isDavComment = lock->is_dav_comment != 0;
			_creationDate = SvnBase::DateTimeFromAprTime(lock->creation_date);
			_expirationDate = SvnBase::DateTimeFromAprTime(lock->expiration_date);
		}

	public:
		property String^ Path
		{
			String^ get()
			{
				if(!_path && _lock)
					_path = SvnBase::Utf8_PtrToString(_lock->path);

				return _path;
			}
		}

		/// <summary>The path the notification is about, translated via <see cref="System::IO::Path::GetFullPath" /></summary>
		/// <remarks>The <see cref="FullPath" /> property contains the path in normalized format; while <see cref="Path" /> returns the exact path from the subversion api</remarks>
		property String^ FullPath
		{
			String^ get()
			{
				if(!_fullPath && _localData && Path)
					_fullPath = System::IO::Path::GetFullPath(Path);

				return _fullPath;
			}
		}

		property String^ Token
		{
			String^ get()
			{
				if(!_token && _lock)
					_token = SvnBase::Utf8_PtrToString(_lock->token);

				return _token;
			}
		}

		property String^ Owner
		{
			String^ get()
			{
				if(!_owner && _lock)
					_owner = SvnBase::Utf8_PtrToString(_lock->owner);

				return _owner;
			}
		}

		property String^ Comment
		{
			String^ get()
			{
				if(!_comment && _lock && _lock->comment)
				{
					_comment = SvnBase::Utf8_PtrToString(_lock->comment);

					if(_comment)
						_comment->Replace("\n", Environment::NewLine);
				}

				return _comment;
			}
		}

		property bool IsWebDavComment
		{
			bool get()
			{
				return _isDavComment;
			}
		}

		property DateTime CreationDate
		{
			DateTime get()
			{
				return _creationDate;
			}
		}

		property DateTime ExpirationDate
		{
			DateTime get()
			{
				return _expirationDate;
			}
		}



	public:
		virtual void Detach(bool keepProperties)
		{
			try
			{
				if(keepProperties)
				{
					GC::KeepAlive(Path);
					GC::KeepAlive(Token);
					GC::KeepAlive(Owner);
					GC::KeepAlive(Comment);
				}
			}
			finally
			{
				_lock = nullptr;
				//__super::Detach(keepProperties);
			}
		}
	};

	public ref class SvnNotifyEventArgs sealed : public SvnEventArgs
	{
		const svn_wc_notify_t *_notify;
		initonly SvnNotifyAction _action;
		initonly SvnNodeKind _nodeKind;
		initonly SvnNotifyState _contentState;
		initonly SvnNotifyState _propertyState;
		initonly SvnLockState _lockState;
		initonly __int64 _revision;
		SvnLockInfo^ _lock;

	internal:
		SvnNotifyEventArgs(const svn_wc_notify_t *notify)
		{
			if(!notify)
				throw gcnew ArgumentNullException("notify");

			_notify = notify;
			_action = (SvnNotifyAction)notify->action;
			_nodeKind = (SvnNodeKind)notify->kind;
			_contentState = (SvnNotifyState)notify->content_state;
			_propertyState = (SvnNotifyState)notify->prop_state;
			_lockState = (SvnLockState)notify->lock_state;
			_revision = notify->revision;

			// TODO: Add support for the Lock member (containing the lock owner etc.)
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
				if(!_path && _notify)
					_path = SvnBase::Utf8_PtrToString(_notify->path);

				return _path;
			}
		}

		/// <summary>The path the notification is about, translated via <see cref="System::IO::Path::GetFullPath" /></summary>
		/// <remarks>The <see cref="FullPath" /> property contains the path in normalized format; while <see cref="Path" /> returns the exact path from the subversion api</remarks>
		property String^ FullPath
		{
			String^ get()
			{
				if(!_fullPath && Path)
					_fullPath = System::IO::Path::GetFullPath(Path);

				return _fullPath;
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
				if(!_mimeType && _notify && _notify->mime_type)
					_mimeType = SvnBase::Utf8_PtrToString(_notify->mime_type);

				return _mimeType;
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
				if(!_lock && _notify && _notify->lock)
					_lock = gcnew SvnLockInfo(_notify->lock, false);

				return _lock;
			}
		}


	public:
		/// <summary>Detaches the SvnEventArgs from the unmanaged storage; optionally keeping the property values for later use</summary>
		/// <description>After this method is called all properties are either stored managed, or are no longer readable</description>
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
				{
					// Use all properties to get them cached in .Net memory
					GC::KeepAlive(Path);
					GC::KeepAlive(MimeType);
					GC::KeepAlive(Error);
					GC::KeepAlive(Lock);
				}

				if(_lock)
					_lock->Detach(keepProperties);
			}
			finally
			{
				_notify = nullptr;
				__super::Detach(keepProperties);
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
		Uri^ _uri;
		initonly __int64 _oodLastCommitRev;
		initonly DateTime _oodLastCommitDate;
		initonly SvnNodeKind _oodLastCommitNodeKind;
		String^ _oodLastCommitAuthor;

	internal:
		SvnStatusEventArgs(String^ path, const svn_wc_status2_t *status)
		{
			if(String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");
			else if(!status)
				throw gcnew ArgumentNullException("status");

			_path = path;
			_status = status;

			_wcContentStatus = (SvnStatus)status->text_status;
			_wcPropertyStatus = (SvnStatus)status->prop_status;
			_wcLocked = status->locked != 0;
			_copied = status->copied != 0;
			_switched = status->switched != 0;
			_reposContentStatus = (SvnStatus)status->repos_text_status;
			_reposPropertyStatus = (SvnStatus)status->repos_prop_status;

			_oodLastCommitRev = status->ood_last_cmt_rev;

			if(status->ood_last_cmt_rev != SVN_INVALID_REVNUM)
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
				if(!_fullPath && Path)
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

		property SvnStatus ReposContentStatus
		{
			SvnStatus get()
			{
				return _reposContentStatus;
			}
		}

		property SvnStatus ReposPropertyStatus
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
				if(!_reposLock && _status && _status->repos_lock)
					_reposLock = gcnew SvnLockInfo(_status->repos_lock, false);

				return _reposLock;
			}
		}

		property Uri^ Uri
		{
			System::Uri^ get()
			{
				if(!_uri && _status && _status->url)
					_uri = gcnew System::Uri(SvnBase::Utf8_PtrToString(_status->url));

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
		property DateTime RemoteUpdateCommitDate
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
				if(!_oodLastCommitAuthor && _status && _status->ood_last_cmt_author && IsRemoteUpdated)
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

	public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
				{
					// Use all properties to get them cached in .Net memory
					GC::KeepAlive(Uri);
					GC::KeepAlive(RemoteLock);
					GC::KeepAlive(RemoteUpdateCommitAuthor);
				}

				if(_reposLock)
					_reposLock->Detach(keepProperties);
			}
			finally
			{
				_status = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};

	public ref class SvnChangeItem sealed
	{
		initonly String^ _path;
		initonly SvnChangeAction _action;
		initonly String^ _copyFromPath;
		initonly __int64 _copyFromRevision;
	protected public:
		SvnChangeItem(String^ path, SvnChangeAction action, String^ copyFromPath, __int64 copyFromRevision)
		{
			if(String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");

			_path = path;
			_action = action;
			_copyFromPath = copyFromPath;
			_copyFromRevision = copyFromPath ? copyFromRevision : -1;
		}

	public:
		property String^ Path
		{
			String^ get()
			{
				return _path;
			}
		}

		property SvnChangeAction Action
		{
			SvnChangeAction get()
			{
				return _action;
			}
		}

		property String^ CopyFromPath
		{
			String^ get()
			{
				return _copyFromPath;
			}
		}

		property __int64 CopyFromRevision
		{
			__int64 get()
			{
				return _copyFromRevision;
			}
		}
	};

	public ref class SvnLogEventArgs : public SvnCancelEventArgs
	{
		svn_log_entry_t* _entry;
		AprPool^ _pool;

		String^ _author;
		DateTime _date;
		String^ _message;
		__int64 _revision;
		bool _hasChildren;

	internal:
		SvnLogEventArgs(svn_log_entry_t *entry, AprPool ^pool)
		{
			if(!entry)
				throw gcnew ArgumentNullException("entry");
			else if(!pool)
				throw gcnew ArgumentNullException("pool");

			_entry = entry;
			_pool = pool;

			apr_time_t when = 0; // Documentation: date must be parsable by svn_time_from_cstring()
			svn_error_t *err = svn_time_from_cstring(&when, entry->date, pool->Handle); // pool is not used at this time (might be for errors in future versions)

			if(!err)
				_date = SvnBase::DateTimeFromAprTime(when);

			_revision = entry->revision;
			_hasChildren = entry->has_children ? true : false;
		}

	private:
		ref class ChangedPathsCollection sealed : System::Collections::ObjectModel::KeyedCollection<String^, SvnChangeItem^>
		{
		protected:
			virtual String^ GetKeyForItem(SvnChangeItem^ item) override
			{
				if(!item)
					throw gcnew ArgumentNullException("item");

				return item->Path;
			}
		};

		ChangedPathsCollection^ _changedPaths;
	public:

		property System::Collections::ObjectModel::KeyedCollection<String^, SvnChangeItem^>^ ChangedPaths
		{
			System::Collections::ObjectModel::KeyedCollection<String^, SvnChangeItem^>^  get()
			{
				if(!_changedPaths && _entry && _pool)
				{
					_changedPaths = gcnew ChangedPathsCollection();

					for (apr_hash_index_t* hi = apr_hash_first(_pool->Handle, _entry->changed_paths); hi; hi = apr_hash_next(hi))
					{
						const char* pKey;
						apr_ssize_t keyLen;
						svn_log_changed_path_t *pChangeInfo;

						apr_hash_this(hi, (const void**)&pKey, &keyLen, (void**)&pChangeInfo);

						SvnChangeItem^ ci = gcnew SvnChangeItem(SvnBase::Utf8_PtrToString(pKey, (int)keyLen),
							(SvnChangeAction)pChangeInfo->action,
							SvnBase::Utf8_PtrToString(pChangeInfo->copyfrom_path),
							pChangeInfo->copyfrom_rev);

						_changedPaths->Add(ci);
					}
				}

				return _changedPaths;
			}
		}

		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}

		property String^ Author
		{
			String^ get()
			{
				if(!_author && _entry && _entry->author)
					_author = SvnBase::Utf8_PtrToString(_entry->author);

				return _author;
			}
		}

		property DateTime Date
		{
			DateTime get()
			{

				return _date;
			}
		}

		property String^ LogMessage
		{
			String^ get()
			{
				if(!_message && _entry && _entry->message)
				{
					_message = SvnBase::Utf8_PtrToString(_entry->message);

					if(_message)
					{
						// Subversion log messages always use \n newlines
						_message = _message->Replace("\n", Environment::NewLine);
					}
				}

				return _message;
			}
		}

		property bool HasChildren
		{
			bool get()
			{
				return _hasChildren;
			}
		}

	public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
				{
					// Use all properties to get them cached in .Net memory
					GC::KeepAlive(ChangedPaths);
					GC::KeepAlive(Author);
					GC::KeepAlive(LogMessage);
				}
			}
			finally
			{
				_entry = nullptr;
				_pool = nullptr;
				__super::Detach(keepProperties);
			}
		}

	};

	public ref class SvnInfoEventArgs : public SvnCancelEventArgs
	{
		const svn_info_t* _info;
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
		String^ _changeList;
		initonly SvnDepth _depth;
		initonly __int64 _wcSize;
		initonly __int64 _size;

	internal:
		SvnInfoEventArgs(String^ path, const svn_info_t* info)
		{
			if(String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");
			else if(!info)
				throw gcnew ArgumentNullException("info");

			_info = info;
			_path = path;
			_rev = info->rev;
			_nodeKind = (SvnNodeKind)info->kind;
			if(info->last_changed_author)
			{
				_lastChangeRev = info->last_changed_rev;
				_lastChangeDate = SvnBase::DateTimeFromAprTime(info->last_changed_date);
			}
			_hasWcInfo = info->has_wc_info != 0;
			_wcSchedule = (SvnSchedule)info->schedule;
			_copyFromRev = info->copyfrom_rev;
			if(_hasWcInfo)
			{
				_contentTime = SvnBase::DateTimeFromAprTime(info->text_time);
				_propertyTime = SvnBase::DateTimeFromAprTime(info->prop_time);
			}

			_depth = (SvnDepth)info->depth;

			if(info->size == (apr_size_t)-1)
				_size = -1;
			else
				_size = info->size;

			if(info->working_size == (apr_size_t)-1)
				_wcSize = -1;
			else
				_wcSize = info->working_size;
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
				if(!_fullPath && Path && HasLocalInfo)
					_fullPath = System::IO::Path::GetFullPath(Path);

				return _fullPath;
			}
		}

		property Uri^ Uri
		{
			System::Uri^ get()
			{
				if(!_uri && _info && _info->URL)
					_uri = gcnew System::Uri(SvnBase::Utf8_PtrToString(_info->URL));

				return _uri;
			}
		}

		property __int64 Revision
		{
			__int64 get()
			{
				return _rev;
			}
		}

		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return _nodeKind;
			}
		}

		/// <summary>Gets the repository root Uri; ending in a '/' at the end</summary>
		/// <remarks>The unmanaged api does not add the '/' at the end, but this makes using <see cref="System::Uri" /> hard</remarks>
		property System::Uri^ RepositoryRoot
		{
			System::Uri^ get()
			{
				if(!_reposRootUri && _info && _info->repos_root_URL)
				{
					// Subversion does not add a / at the end; but to use the Uri infrastructure we require a /
					_reposRootUri = gcnew System::Uri(SvnBase::Utf8_PtrToString(_info->repos_root_URL) + "/");
				}

				return _reposRootUri;
			}
		}

		property Guid RepositoryUuid
		{
			Guid get()
			{
				if(!_reposUuid && _info && _info->repos_UUID)
					_reposUuid = SvnBase::Utf8_PtrToString(_info->repos_UUID);

				return _reposUuid ? Guid(_reposUuid) : Guid::Empty;
			}
		}

		property __int64 LastChangeRevision
		{
			__int64 get()
			{
				return _lastChangeRev;
			}
		}

		property DateTime LastChangeDate
		{
			DateTime get()
			{
				return _lastChangeDate;
			}
		}

		property String^ LastChangeAuthor
		{
			String^ get()
			{
				if(!_lastChangeAuthor && _info && _info->last_changed_author)
					_lastChangeAuthor = SvnBase::Utf8_PtrToString(_info->last_changed_author);

				return _lastChangeAuthor;
			}
		}

		property SvnLockInfo^ Lock
		{
			SvnLockInfo^ get()
			{
				if(!_lock && _info && _info->lock)
					_lock = gcnew SvnLockInfo(_info->lock, HasLocalInfo);

				return _lock;
			}
		}

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
				if(!_copyFromUri && _info && _info->copyfrom_url && HasLocalInfo)
					_copyFromUri = gcnew System::Uri(SvnBase::Utf8_PtrToString(_info->copyfrom_url));

				return _copyFromUri;
			}
		}

		property __int64 CopyFromRev
		{
			__int64 get()
			{
				return _copyFromRev;
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
				if(!_checksum && _info && _info->checksum && HasLocalInfo)
					_checksum = SvnBase::Utf8_PtrToString(_info->checksum);

				return _checksum;
			}
		}

		property String^ ConflictOld
		{
			String^ get()
			{
				if(!_conflict_old && _info && _info->conflict_old && HasLocalInfo)
					_conflict_old = SvnBase::Utf8_PtrToString(_info->conflict_old);

				return _conflict_old;
			}
		}

		property String^ ConflictNew
		{
			String^ get()
			{
				if(!_conflict_new && _info && _info->conflict_new && HasLocalInfo)
					_conflict_new = SvnBase::Utf8_PtrToString(_info->conflict_new);

				return _conflict_new;
			}
		}

		property String^ ConflictWork
		{
			String^ get()
			{
				if(!_conflict_wrk && _info && _info->conflict_wrk && HasLocalInfo)
					_conflict_wrk = SvnBase::Utf8_PtrToString(_info->conflict_wrk);

				return _conflict_wrk;
			}
		}

		property String^ PropertyEditFile
		{
			String^ get()
			{
				if(!_prejfile && _info && _info->prejfile && HasLocalInfo)
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
				if(!_changeList && _info && _info->changelist)
					_changeList = SvnBase::Utf8_PtrToString(_info->changelist);

				return _changeList;
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

	public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
				{
					// Use all properties to get them cached in .Net memory
					GC::KeepAlive(Path);
					GC::KeepAlive(Uri);
					GC::KeepAlive(RepositoryRoot);
					GC::KeepAlive(RepositoryUuid);
					GC::KeepAlive(LastChangeAuthor);
					GC::KeepAlive(Lock);
					GC::KeepAlive(CopyFromUri);
					GC::KeepAlive(Checksum);
					GC::KeepAlive(ConflictOld);
					GC::KeepAlive(ConflictNew);
					GC::KeepAlive(ConflictWork);
					GC::KeepAlive(PropertyEditFile);
					GC::KeepAlive(ChangeList);
				}

				if(_lock)
					_lock->Detach(keepProperties);
			}
			finally
			{
				_info = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};

	public ref class SvnDirEntry : public ISvnDetachable
	{
		const svn_dirent_t *_entry;
		initonly SvnNodeKind _nodeKind;
		initonly __int64 _fileSize;
		initonly bool _hasProperties;
		initonly __int64 _rev;
		initonly DateTime _time;
		String^ _author;
	internal:
		SvnDirEntry(const svn_dirent_t *entry)
		{
			if(!entry)
				throw gcnew ArgumentNullException("entry");

			_entry = entry;
			_nodeKind = (SvnNodeKind)entry->kind;
			_fileSize = entry->size;
			_hasProperties = (entry->has_props != 0);
			_rev = entry->created_rev;
			_time = (entry->time != 0) ? SvnBase::DateTimeFromAprTime(entry->time) : DateTime::MinValue;
		}
	public:
		property SvnNodeKind Kind
		{
			SvnNodeKind get()
			{
				return _nodeKind;
			}
		}

		property __int64 FileSize
		{
			__int64 get()
			{
				return _fileSize;
			}
		}

		property bool HasProperties
		{
			bool get()
			{
				return _hasProperties;
			}
		}

		property __int64 Revision
		{
			__int64 get()
			{
				return _rev;
			}
		}

		property DateTime Time
		{
			DateTime get()
			{
				return _time;
			}
		}
		property String^ Author
		{
			String^ get()
			{
				if(!_author && _entry->last_author)
					_author = SvnBase::Utf8_PtrToString(_entry->last_author);

				return _author;
			}
		}

	public:
		virtual void Detach(bool keepProperties)
		{
			try
			{
				if(keepProperties)
				{
					GC::KeepAlive(Author);
				}

			}
			finally
			{
				_entry = nullptr;

				//__super::Detach(keepProperties);
			}
		}
	};


	public ref class SvnListEventArgs : public SvnCancelEventArgs
	{
		initonly String^ _path;
		const char* _pAbsPath;
		const svn_lock_t *_pLock;
		const svn_dirent_t *_pDirEnt;

		String^ _absPath;
		SvnLockInfo^ _lock;
		SvnDirEntry^ _entry;

	internal:
		SvnListEventArgs(const char *path, const svn_dirent_t *dirent, const svn_lock_t *lock, const char *abs_path)
		{
			if(!path)
				throw gcnew ArgumentNullException("path");
			else if(!abs_path)
				throw gcnew ArgumentNullException("abs_path");

			_path = SvnBase::Utf8_PtrToString(path);

			_pDirEnt = dirent;
			_pLock = lock;
			_pAbsPath = abs_path;
		}

	public:
		property String^ ItemPath
		{
			String^ get()
			{
				return _path;
			}
		}

		property String^ BasePath
		{
			String^ get()
			{
				if(!_absPath && _pAbsPath)
					_absPath = SvnBase::Utf8_PtrToString(_pAbsPath);

				return _absPath;
			}
		}

		property SvnLockInfo^ Lock
		{
			SvnLockInfo^ get()
			{
				if(!_lock && _pLock)
					_lock = gcnew SvnLockInfo(_pLock, false);

				return _lock;
			}
		}

		property SvnDirEntry^ Entry
		{
			SvnDirEntry^ get()
			{
				if(!_entry && _pDirEnt)
					_entry = gcnew SvnDirEntry(_pDirEnt);

				return _entry;
			}
		}


	public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
				{
					GC::KeepAlive(ItemPath);
					GC::KeepAlive(BasePath);
					GC::KeepAlive(Lock);
					GC::KeepAlive(Entry);
				}

				if(_lock)
					_lock->Detach(keepProperties);
				if(_entry)
					_entry->Detach(keepProperties);
			}
			finally
			{
				_pAbsPath = nullptr;
				_pLock = nullptr;
				_pDirEnt = nullptr;

				__super::Detach(keepProperties);
			}
		}
	};

	public ref class SvnPropertyListEventArgs : public SvnCancelEventArgs
	{
		initonly String^ _path;
		apr_hash_t* _propHash;
		IDictionary<String^, Object^>^ _properties;
		AprPool^ _pool;

	internal:
		SvnPropertyListEventArgs(const char *path, apr_hash_t* prop_hash, AprPool ^pool)
		{
			if(!path)
				throw gcnew ArgumentNullException("path");
			else if(!prop_hash)
				throw gcnew ArgumentNullException("prop_hash");
			else if(!pool)
				throw gcnew ArgumentNullException("pool");

			_path = SvnBase::Utf8_PtrToString(path);
			_propHash = prop_hash;
			_pool = pool;
		}

	public:
		property String^ Path
		{
			String^ get()
			{
				return _path;
			}
		}

		property IDictionary<String^, Object^>^ Properties
		{
			IDictionary<String^, Object^>^ get()
			{
				if(!_properties && _propHash && _pool)
					_properties = SvnBase::CreatePropertyDictionary(_propHash, _pool);

				return _properties;
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
					GC::KeepAlive(Properties);
					//					GC::KeepAlive(Entry);
				}
			}
			finally
			{
				_propHash = nullptr;
				_pool = nullptr;

				__super::Detach(keepProperties);
			}
		}
	};

	public ref class SvnRevisionPropertyListEventArgs : public SvnCancelEventArgs
	{
		initonly String^ _path;
		apr_hash_t* _propHash;
		IDictionary<String^, Object^>^ _properties;
		AprPool^ _pool;

	internal:
		SvnRevisionPropertyListEventArgs(apr_hash_t* prop_hash, AprPool ^pool)
		{
			if(!prop_hash)
				throw gcnew ArgumentNullException("prop_hash");
			else if(!pool)
				throw gcnew ArgumentNullException("pool");

			_propHash = prop_hash;
			_pool = pool;
		}

	public:
		property IDictionary<String^, Object^>^ Properties
		{
			IDictionary<String^, Object^>^ get()
			{
				if(!_properties && _propHash && _pool)
					_properties = SvnBase::CreatePropertyDictionary(_propHash, _pool);
				
				return _properties;
			}
		}

	public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
				{
					GC::KeepAlive(Properties);
					//					GC::KeepAlive(Entry);
				}
			}
			finally
			{
				_propHash = nullptr;
				_pool = nullptr;

				__super::Detach(keepProperties);
			}
		}
	};

	public ref class SvnListChangeListEventArgs : public SvnCancelEventArgs
	{
		initonly String^ _path;

	internal:
		SvnListChangeListEventArgs(const char *path)
		{
			if(!path)
				throw gcnew ArgumentNullException("path");

			_path = SvnBase::Utf8_PtrToString(path)->Replace('/', System::IO::Path::DirectorySeparatorChar);
		}

	public:
		property String^ Path
		{
			String^ get()
			{
				return _path;
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
				}
			}
			finally
			{
				__super::Detach(keepProperties);
			}
		}
	};

	public ref class SvnDiffSummaryEventArgs : public SvnCancelEventArgs
	{
		const svn_client_diff_summarize_t *_diffSummary;
		String^ _path;
		initonly bool _propertiesChanged;
		initonly SvnNodeKind _nodeKind;
		initonly SvnDiffKind _diffKind;
	internal:
		SvnDiffSummaryEventArgs(const svn_client_diff_summarize_t *diffSummary)
		{
			if(!diffSummary)
				throw gcnew ArgumentNullException("diffSummary");

			_diffSummary = diffSummary;
			_propertiesChanged = (0 != diffSummary->prop_changed);
			_nodeKind = (SvnNodeKind)diffSummary->node_kind;
			_diffKind = (SvnDiffKind)diffSummary->summarize_kind;
		}


	public:
		property String^ Path
		{
			String^ get()
			{
				if(!_path && _diffSummary)
					_path = SvnBase::Utf8_PtrToString(_diffSummary->path);

				return _path;
			}
		}

		property bool PropertiesChanged
		{
			bool get()
			{
				return _propertiesChanged;
			}
		}

		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return _nodeKind;
			}
		}

		property SvnDiffKind DiffKind
		{
			SvnDiffKind get()
			{
				return _diffKind;
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
				}
			}
			finally
			{
				_diffSummary = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};

	public ref class SvnBlameEventArgs : public SvnCancelEventArgs
	{
		__int64 _revision;
		__int64 _lineNr;
		const char* _pcAuthor;
		const char* _pcLine;
		DateTime _date;
		String^ _author;
		String^ _line;
		__int64 _mergedRevision;
		const char* _pcMergedAuthor;
		const char* _pcMergedPath;
		DateTime _mergedDate;
		String^ _mergedAuthor;
		String^ _mergedPath;

	internal:
		SvnBlameEventArgs(__int64 revision, __int64 lineNr, const char* author, const char* date, 
			svn_revnum_t merged_revision, const char *merged_author, const char *merged_date, 
			const char *merged_path, const char* line, AprPool^ pool)
		{
			if(!pool)
				throw gcnew ArgumentNullException("pool");
			else if(!author)
				throw gcnew ArgumentNullException("author");
			else if(!date)
				throw gcnew ArgumentNullException("date");
			else if(!line)
				throw gcnew ArgumentNullException("line");

			_revision = revision;
			_lineNr = lineNr;
			_pcAuthor = author;
			_pcLine = line;

			apr_time_t when = 0; // Documentation: date must be parsable by svn_time_from_cstring()
			svn_error_t *err = svn_time_from_cstring(&when, date, pool->Handle); // pool is not used at this time (might be for errors in future versions)

			if(!err)
				_date = SvnBase::DateTimeFromAprTime(when);

			_mergedRevision = merged_revision;
			_pcMergedAuthor = merged_author;
			_pcMergedPath = merged_path;

			if(merged_date)
			{
				err = svn_time_from_cstring(&when, merged_date, pool->Handle);

				if(!err)
					_mergedDate = SvnBase::DateTimeFromAprTime(when);
			}
		}

	public:
		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}

		property __int64 LineNumber
		{
			__int64 get()
			{
				return _lineNr;
			}
		}
		property String^ Author
		{
			String^ get()
			{
				if(!_author && _pcAuthor)
					_author = SvnBase::Utf8_PtrToString(_pcAuthor);

				return _author;
			}
		}

		property String^ Line
		{
			String^ get()
			{
				if(!_line && _pcLine)
				{
					try
					{
						_line = SvnBase::Utf8_PtrToString(_pcLine);
					}
					catch(Exception^)
					{
						_line = SharpSvnStrings::NonUtf8ConvertableLine;
					}
				}

				return _line;
			}
		}

		property String^ MergedAuthor
		{
			String^ get()
			{
				if(!_mergedAuthor && _pcMergedAuthor)
					_mergedAuthor = SvnBase::Utf8_PtrToString(_pcMergedAuthor);

				return _mergedAuthor;
			}
		}

		property String^ MergedPath
		{
			String^ get()
			{
				if(!_mergedPath && _pcMergedPath)
					_mergedPath = SvnBase::Utf8_PtrToString(_pcMergedPath);

				return _mergedPath;
			}
		}

		property DateTime MergedDate
		{
			DateTime get()
			{
				return _mergedDate; 
			}
		}

		property __int64 MergedRevision
		{
			__int64 get()
			{
				return _mergedRevision;
			}
		}

		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
				{
					GC::KeepAlive(Author);
					GC::KeepAlive(Line);
					GC::KeepAlive(MergedAuthor);
					GC::KeepAlive(MergedPath);
				}
			}
			finally
			{
				_pcAuthor = nullptr;
				_pcLine = nullptr;
				_pcMergedAuthor = nullptr;
				_pcMergedPath = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};

}
