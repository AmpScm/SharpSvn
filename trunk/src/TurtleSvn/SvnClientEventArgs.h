
#pragma once

namespace TurtleSvn {

	ref class SvnException;

	public ref class SvnClientEventArgs abstract : public System::EventArgs
	{
		apr_pool_t *_pool;

	internal:
		SvnClientEventArgs(apr_pool_t *pool)
		{
			_pool = pool;
		}

	protected:
		SvnClientEventArgs()
		{
		}

	public:
		/// <summary>Detaches the SvnClientEventArgs from the unmanaged storage; optionally keeping the property values for later use</summary>
		/// <description>After this method is called all properties are either stored managed, or are no longer readable</description>
		virtual void Detach(bool keepProperties)
		{
			_pool = nullptr;
		}
	};

	public ref class SvnClientCancelEventArgs : public SvnClientEventArgs
	{
		bool _cancel;

	public:
		SvnClientCancelEventArgs()
		{
		}

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


	internal:
		SvnClientProgressEventArgs(__int64 progress, __int64 totalProgress, apr_pool_t *pool)
			: SvnClientEventArgs(pool)
		{
			_progress = progress;
			_totalProgress = totalProgress;
		}

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

	public ref class SvnClientCommitLogEventArgs : public SvnClientEventArgs
	{
		bool _cancel;
	internal:
		SvnClientCommitLogEventArgs()
		{
		}

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

	public enum class SvnNotifyAction
	{
		Add				= svn_wc_notify_add, 
		Copy			= svn_wc_notify_copy, 
		Delete			= svn_wc_notify_delete, 
		Restore			= svn_wc_notify_restore, 
		Revert			= svn_wc_notify_revert, 
		FailedRevert	= svn_wc_notify_failed_revert, 
		Resolved		= svn_wc_notify_resolved, 
		Skip			= svn_wc_notify_skip, 
		UpdateDelete	= svn_wc_notify_update_delete, 
		UpdateAdd		= svn_wc_notify_update_add, 
		UpdateData		= svn_wc_notify_update_update, 
		UpdateCompleted = svn_wc_notify_update_completed, 
		UpdateExternal  = svn_wc_notify_update_external, 
		StatusCompleted = svn_wc_notify_status_completed, 
		StatusExternal	= svn_wc_notify_status_external, 
		CommitModified	= svn_wc_notify_commit_modified, 
		CommitAdded		= svn_wc_notify_commit_added, 
		CommitDeleted	= svn_wc_notify_commit_deleted, 
		CommitReplaced	= svn_wc_notify_commit_replaced, 
		CommitBeforeDelta=svn_wc_notify_commit_postfix_txdelta, 
		BlameRevision	= svn_wc_notify_blame_revision, 
		Locked			= svn_wc_notify_locked, 
		LockUnlocked	= svn_wc_notify_unlocked, 
		LockFailed		= svn_wc_notify_failed_lock, 
		LockFailedUnlock=svn_wc_notify_failed_unlock 

		// TODO: Add new items for Subversion 1.5+
	};

	public enum class SvnNodeKind
	{
		None		= svn_node_none, 
		File		= svn_node_file, 
		Directory	= svn_node_dir, 
		Unknown		= svn_node_unknown
	};

	public enum class SvnNotifyState
	{
		None		= svn_wc_notify_state_inapplicable, 
		Unknown		= svn_wc_notify_state_unknown, 
		Unchanged	= svn_wc_notify_state_unchanged, 
		Missing		= svn_wc_notify_state_missing, 
		Obstructed	= svn_wc_notify_state_obstructed, 
		Changed		= svn_wc_notify_state_changed, 
		Merged		= svn_wc_notify_state_merged, 
		Conflicted	= svn_wc_notify_state_conflicted 
	};

	public enum class SvnLockState
	{
		Unchanged	= svn_wc_notify_lock_state_unchanged,
		Locked		= svn_wc_notify_lock_state_locked,
		Unlocked	= svn_wc_notify_lock_state_unlocked
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
		
	internal:
		SvnClientNotifyEventArgs(const svn_wc_notify_t *notify, apr_pool_t *pool)
			: SvnClientEventArgs(pool)
		{
			if(!notify)
				throw gcnew ArgumentNullException("notify");

			_action = (SvnNotifyAction)notify->action;
			_nodeKind = (SvnNodeKind)notify->kind;
			_contentState = (SvnNotifyState)notify->content_state;
			_propertyState = (SvnNotifyState)notify->prop_state;
			_lockState = (SvnLockState)notify->lock_state;
			_revision = notify->revision;

			// TODO: Add support for the Lock member (containing the lock owner etc.)
		}

	private:
		String^ _path;
		String^ _mimeType;
		SvnException^ _exception;
	public:
		property String^ Path
		{
			String^ get()
			{
				if(!_path && _notify)
					_path = SvnBase::Utf8_PtrToString(_notify->path);

				return _path;
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
				}
			}
			finally
			{
				_notify = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};
}
