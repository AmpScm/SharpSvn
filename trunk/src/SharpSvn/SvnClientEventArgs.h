
#pragma once

#include "SvnEnums.h"
#include "SvnCommitItem.h"

namespace SharpSvn {
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IDictionary;
	using System::Collections::Generic::IList;
	using System::Collections::Generic::SortedList;

	ref class SvnException;

	public ref class SvnClientEventArgs abstract : public System::EventArgs
	{
	protected:
		SvnClientEventArgs()
		{
		}

	public:
		/// <summary>Detaches the SvnClientEventArgs from the unmanaged storage; optionally keeping the property values for later use</summary>
		/// <description>After this method is called all properties are either stored managed, or are no longer readable</description>
		virtual void Detach(bool keepProperties)
		{
			UNUSED_ALWAYS(keepProperties);
		}
	};

	public ref class SvnClientCancelEventArgs : public SvnClientEventArgs
	{
		bool _cancel;

	public:
		SvnClientCancelEventArgs()
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

	public ref class SvnClientProgressEventArgs : public SvnClientEventArgs
	{
		initonly __int64 _progress;
		initonly __int64 _totalProgress;

	public:
		SvnClientProgressEventArgs(__int64 progress, __int64 totalProgress)
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

	public ref class SvnClientBeforeCommitEventArgs : public SvnClientEventArgs
	{
		AprPool^ _pool;
		const apr_array_header_t *_commitItems;
		bool _cancel;
		String^ _logMessage;
		IList<SvnCommitItem^>^ _items;
	internal:
		SvnClientBeforeCommitEventArgs(const apr_array_header_t *commitItems, AprPool^ pool);

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

	public ref class SvnClientConflictResolveEventArgs : public SvnClientCancelEventArgs
	{
		AprPool^ _pool;
		const svn_wc_conflict_description_t* _description;
		SvnConflictResult _result;

	internal:
		SvnClientConflictResolveEventArgs(const svn_wc_conflict_description_t *description, AprPool^ pool)
		{
			if(!description)
				throw gcnew ArgumentNullException("description");
			else if(!pool)
				throw gcnew ArgumentNullException("pool");

			_description = description;
			_pool = pool;
			_result = SvnConflictResult::Conflicted;
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
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
				{
				}
			}
			finally
			{
				_description = nullptr;
				_pool = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};

	public ref class SvnLockInfo : public SvnClientEventArgs
	{
		const svn_lock_t *_lock;
		String^ _path;
		String^ _fullPath;
		String^ _token;
		String^ _owner;
		String^ _comment;
		initonly bool _isDavComment;
		initonly DateTime _creationDate;
		initonly DateTime _expirationDate;
	internal:
		SvnLockInfo(const svn_lock_t *lock)
		{
			if(!lock)
				throw gcnew ArgumentNullException("lock");

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
				if(!_fullPath && Path)
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
				if(!_comment && _lock)
					_comment = SvnBase::Utf8_PtrToString(_lock->comment);

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
		virtual void Detach(bool keepProperties) override
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
				__super::Detach(keepProperties);
			}
		}
	};

	public ref class SvnClientNotifyEventArgs : public SvnClientEventArgs
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
		SvnClientNotifyEventArgs(const svn_wc_notify_t *notify)
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
					_lock = gcnew SvnLockInfo(_notify->lock);

				return _lock;
			}
		}


	public:
		/// <summary>Detaches the SvnClientEventArgs from the unmanaged storage; optionally keeping the property values for later use</summary>
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

	public ref class SvnStatusEventArgs : public SvnClientEventArgs
	{
		initonly String^ _path;
		const svn_wc_status2_t *_status;
		String^ _fullPath;
		initonly SvnWcStatus _wcContentStatus;
		initonly SvnWcStatus _wcPropertyStatus;
		initonly bool _wcLocked;
		initonly bool _copied;
		initonly bool _switched;
		initonly SvnWcStatus _reposContentStatus;
		initonly SvnWcStatus _reposPropertyStatus;
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

			_wcContentStatus = (SvnWcStatus)status->text_status;
			_wcPropertyStatus = (SvnWcStatus)status->prop_status;
			_wcLocked = status->locked != 0;
			_copied = status->copied != 0;
			_switched = status->switched != 0;
			_reposContentStatus = (SvnWcStatus)status->repos_text_status;
			_reposPropertyStatus = (SvnWcStatus)status->repos_prop_status;

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
		property SvnWcStatus WcContentStatus
		{
			SvnWcStatus get()
			{
				return _wcContentStatus;
			}
		}

		/// <summary>Property status in working copy</summary>
		property SvnWcStatus WcPropertyStatus
		{
			SvnWcStatus get()
			{
				return _wcPropertyStatus;
			}
		}

		/// <summary>Gets a boolean indicating whether the workingcopy is locked</summary>
		property bool WcLocked
		{
			bool get()
			{
				return _wcLocked;
			}
		}

		/// <summary>Gets a boolean indicating whether the file is copied in the working copy</summary>
		/// <remarks>A file or directory can be 'copied' if it's scheduled for addition-with-history 
		/// (or part of a subtree that is scheduled as such.).</remarks>
		property bool WcCopied
		{
			bool get()
			{
				return _copied;
			}
		}

		/// <summary>Gets a boolean indicating whether the file is switched in the working copy</summary>
		property bool WcSwitched
		{
			bool get()
			{
				return _switched;
			}
		}

		property SvnWcStatus ReposContentStatus
		{
			SvnWcStatus get()
			{
				return _reposContentStatus;
			}
		}

		property SvnWcStatus ReposPropertyStatus
		{
			SvnWcStatus get()
			{
				return _reposPropertyStatus;
			}
		}

		property SvnLockInfo^ ReposLock
		{
			SvnLockInfo^ get()
			{
				if(!_reposLock && _status && _status->repos_lock)
					_reposLock = gcnew SvnLockInfo(_status->repos_lock);

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

		/// <summary>Gets the out of date status of the item; if true the Ood* properties are set</summary>
		property bool IsOutOfDate
		{
			bool get()
			{
				return _oodLastCommitRev != SVN_INVALID_REVNUM;
			}
		}

		/// <summary>Out of Date: Last commit version of the item</summary>
		property __int64 OodLastCommitRevision
		{
			__int64 get()
			{
				return _oodLastCommitRev;
			}
		}

		/// <summary>Out of Date: Last commit date of the item</summary>
		property DateTime OodLastCommitDate
		{
			DateTime get()
			{
				return _oodLastCommitDate;
			}
		}

		/// <summary>Out of Date: Gets the author of the OutOfDate commit</summary>
		property String^ OodCommitAuthor
		{
			String^ get()
			{
				if(!_oodLastCommitAuthor && _status && _status->ood_last_cmt_author && IsOutOfDate)
					_oodLastCommitAuthor = SvnBase::Utf8_PtrToString(_status->ood_last_cmt_author);

				return _oodLastCommitAuthor;
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
					GC::KeepAlive(ReposLock);
					GC::KeepAlive(OodCommitAuthor);
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

	public ref class SvnLogChangeItem sealed
	{
		initonly String^ _path;
		initonly SvnChangeAction _action;
		initonly String^ _copyFromPath;
		initonly __int64 _copyFromRevision;
	protected public:
		SvnLogChangeItem(String^ path, SvnChangeAction action, String^ copyFromPath, __int64 copyFromRevision)
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

	public ref class SvnLogEventArgs : public SvnClientCancelEventArgs
	{
		svn_log_entry_t* _entry;
		AprPool^ _pool;

		SortedList<String^, SvnLogChangeItem^>^ _changedPaths;		
		String^ _author;
		DateTime _date;
		String^ _message;
		__int64 _revision;
		__int64 _nLogChildren;		

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
			_nLogChildren = entry->nbr_children;
		}

	public:
		property IDictionary<String^, SvnLogChangeItem^>^ ChangedPaths
		{
			IDictionary<String^, SvnLogChangeItem^>^  get()
			{
				if(!_changedPaths && _entry && _pool)
				{
					_changedPaths = gcnew SortedList<String^, SvnLogChangeItem^>();

					for (apr_hash_index_t* hi = apr_hash_first(_pool->Handle, _entry->changed_paths); hi; hi = apr_hash_next(hi))
					{
						const char* pKey;
						apr_ssize_t keyLen;
						svn_log_changed_path_t *pChangeInfo;

						apr_hash_this(hi, (const void**)&pKey, &keyLen, (void**)&pChangeInfo);

						SvnLogChangeItem^ ci = gcnew SvnLogChangeItem(SvnBase::Utf8_PtrToString(pKey, (int)keyLen), 
							(SvnChangeAction)pChangeInfo->action, 
							SvnBase::Utf8_PtrToString(pChangeInfo->copyfrom_path),
							pChangeInfo->copyfrom_rev);

						_changedPaths->Add(ci->Path, ci);
					}
				}

				return safe_cast<IDictionary<String^, SvnLogChangeItem^>^>(_changedPaths);
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

		property String^ Message
		{
			String^ get()
			{
				if(!_message && _entry && _entry->message)
				{
					_message = SvnBase::Utf8_PtrToString(_entry->message);

					if(_message)
						_message = _message->Replace("\n", Environment::NewLine);
				}

				return _message;
			}
		}

		property __int64 LogChildren
		{
			__int64 get()
			{
				return _nLogChildren;
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
					GC::KeepAlive(Message);
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

	public ref class SvnInfoEventArgs : public SvnClientCancelEventArgs
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
		SvnWcSchedule _wcSchedule;
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
			_wcSchedule = (SvnWcSchedule)info->schedule;
			_copyFromRev = info->copyfrom_rev;
			if(_hasWcInfo)
			{
				_contentTime = SvnBase::DateTimeFromAprTime(info->text_time);
				_propertyTime = SvnBase::DateTimeFromAprTime(info->prop_time);
			}

			_depth = (SvnDepth)info->depth;
			_wcSize = info->working_size;
			_size = info->size;
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
		property System::Uri^ ReposRoot
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

		property Guid ReposUuid
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
					_lock = gcnew SvnLockInfo(_info->lock);

				return _lock;
			}
		}

		property bool HasWcInfo
		{
			bool get()
			{
				return _hasWcInfo;
			}
		}

		property SvnWcSchedule Schedule
		{
			SvnWcSchedule get()
			{
				return HasWcInfo ? _wcSchedule : SvnWcSchedule::None;
			}
		}

		property System::Uri^ CopyFromUri
		{
			System::Uri^ get()
			{
				if(!_copyFromUri && _info && _info->copyfrom_url && HasWcInfo)
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
				if(!_checksum && _info && _info->checksum && HasWcInfo)
					_checksum = SvnBase::Utf8_PtrToString(_info->checksum);

				return _checksum;
			}
		}

		property String^ ConflictOld
		{
			String^ get()
			{
				if(!_conflict_old && _info && _info->conflict_old && HasWcInfo)
					_conflict_old = SvnBase::Utf8_PtrToString(_info->conflict_old);

				return _conflict_old;
			}
		}

		property String^ ConflictNew
		{
			String^ get()
			{
				if(!_conflict_new && _info && _info->conflict_new && HasWcInfo)
					_conflict_new = SvnBase::Utf8_PtrToString(_info->conflict_new);

				return _conflict_new;
			}
		}

		property String^ ConflictWork
		{
			String^ get()
			{
				if(!_conflict_wrk && _info && _info->conflict_wrk && HasWcInfo)
					_conflict_wrk = SvnBase::Utf8_PtrToString(_info->conflict_wrk);

				return _conflict_wrk;
			}
		}

		property String^ PropertyEditFile
		{
			String^ get()
			{
				if(!_prejfile && _info && _info->prejfile && HasWcInfo)
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
					GC::KeepAlive(ReposRoot);
					GC::KeepAlive(ReposUuid);
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

	public ref class SvnDirEntry : public SvnClientEventArgs
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
		virtual void Detach(bool keepProperties) override
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

				__super::Detach(keepProperties);
			}
		}
	};


	public ref class SvnListEventArgs : public SvnClientCancelEventArgs
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
		property String^ Path
		{
			String^ get()
			{
				return _path;
			}
		}

		property String^ AbsPath
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
					_lock = gcnew SvnLockInfo(_pLock);

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
					GC::KeepAlive(AbsPath);
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

	public ref class SvnPropertyListEventArgs : public SvnClientCancelEventArgs
	{
		initonly String^ _path;
		apr_hash_t* _propHash;
		SortedList<String^, Object^>^ _properties;
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
				if(!_properties && _propHash)
				{
					_properties = gcnew SortedList<String^, Object^>();

					for (apr_hash_index_t* hi = apr_hash_first(_pool->Handle, _propHash); hi; hi = apr_hash_next(hi))
					{
						const char* pKey;
						apr_ssize_t keyLen;
						const svn_string_t *propVal;

						apr_hash_this(hi, (const void**)&pKey, &keyLen, (void**)&propVal);

						String^ key = SvnBase::Utf8_PtrToString(pKey);
						Object^ value = SvnBase::PtrToStringOrByteArray(propVal->data, propVal->len);

						_properties->Add(key, value);
					}
				}

				return safe_cast<IDictionary<String^, Object^>^>(_properties);
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

	public ref class SvnListChangeListEventArgs : public SvnClientCancelEventArgs
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
}
