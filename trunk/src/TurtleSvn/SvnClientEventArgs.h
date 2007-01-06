
#pragma once

#include "SvnEnums.h"

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
