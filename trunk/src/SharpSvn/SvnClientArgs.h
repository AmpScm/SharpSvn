
#pragma once

namespace SharpSvn {
	ref class SvnException;

	public ref class SvnClientArgs abstract
	{
		event EventHandler<SvnClientCancelEventArgs^>^		Cancel;
		event EventHandler<SvnClientProgressEventArgs^>^	Progress;
		event EventHandler<SvnClientNotifyEventArgs^>^		Notify;
		bool _noThrowOnError;
		SvnException^ _exception;

	protected public:
		SvnClientArgs()
		{
		}

		virtual void OnCancel(SvnClientCancelEventArgs^ e)
		{
			Cancel(this, e);
		}

		virtual void OnProgress(SvnClientProgressEventArgs^ e)
		{
			Progress(this, e);
		}

		virtual void OnNotify(SvnClientNotifyEventArgs^ e)
		{
			Notify(this, e);
		}

	public:
		property bool ThrowOnError
		{
			bool get()
			{
				return !_noThrowOnError;
			}
			void set(bool value)
			{
				_noThrowOnError = !value;
			}
		}

		property SvnException^ Exception
		{
			SvnException^ get()
			{
				return _exception;
			}
		}

	internal:
		bool HandleResult(svn_error_t *error);
	};

	public ref class SvnClientArgsWithCommit : public SvnClientArgs
	{
		event EventHandler<SvnClientBeforeCommitEventArgs^>^ BeforeCommit;
		String^ _commitMessage;

	protected:
		SvnClientArgsWithCommit()
		{
		}

	protected public:
		virtual void OnBeforeCommit(SvnClientBeforeCommitEventArgs^ e)
		{
			if(CommitMessage && !e->LogMessage)
				e->LogMessage = CommitMessage;

			BeforeCommit(this, e);
		}

	public:
		property String^ CommitMessage
		{
			String^ get()
			{
				return _commitMessage;
			}

			void set(String^ value)
			{
				_commitMessage = value;
			}
		}
	};

	public ref class SvnCleanUpArgs : public SvnClientArgs
	{
	public:
		SvnCleanUpArgs()
		{
		}
	};

	public ref class SvnCheckOutArgs : public SvnClientArgs
	{
		bool _notRecursive;
		bool _ignoreExternals;
		SvnRevision^ _revision;
		bool _allowUnversionedObstructions;
	public:
		SvnCheckOutArgs()
		{
			_revision = SvnRevision::Head;
		}

		property bool NotRecursive
		{
			bool get()
			{
				return _notRecursive;
			}
			void set(bool value)
			{
				_notRecursive = value;
			}
		}

		property bool IgnoreExternals
		{
			bool get()
			{
				return _ignoreExternals;
			}
			void set(bool value)
			{
				_ignoreExternals = value;
			}
		}

		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
			void set(SvnRevision^ value)
			{
				if(value)
					_revision = value;
				else
					_revision = SvnRevision::Head;
			}
		}

		[Obsolete("Available from Svn 1.5; currently ignored")]
		property bool AllowUnversionedObstructions
		{
			bool get()
			{
				return _allowUnversionedObstructions;
			}
			void set(bool value)
			{
				_allowUnversionedObstructions = value;
			}
		}
	};

	public ref class SvnUpdateArgs : public SvnClientArgs
	{
		bool _notRecursive;
		bool _ignoreExternals;
		bool _allowUnversionedObstructions;
		SvnRevision^ _revision;
	public:
		SvnUpdateArgs()
		{
			_revision = SvnRevision::Head;
		}

		property bool NotRecursive
		{
			bool get()
			{
				return _notRecursive;
			}
			void set(bool value)
			{
				_notRecursive = value;
			}
		}

		property bool IgnoreExternals
		{
			bool get()
			{
				return _ignoreExternals;
			}
			void set(bool value)
			{
				_ignoreExternals = value;
			}
		}

		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
			void set(SvnRevision^ value)
			{
				if(value)
					_revision = value;
				else
					_revision = SvnRevision::Head;
			}
		}

		[Obsolete("Available from Svn 1.5; currently ignored")]
		property bool AllowUnversionedObstructions
		{
			bool get()
			{
				return _allowUnversionedObstructions;
			}
			void set(bool value)
			{
				_allowUnversionedObstructions = value;
			}
		}
	};

	public enum class SvnEolStyle
	{
		Native,
		CrLf,
		Lf,
		Cr
	};	

	public ref class SvnExportArgs : public SvnClientArgs
	{
		bool _notRecursive;
		bool _ignoreExternals;
		bool _overwrite;
		SvnRevision^ _revision;
		SvnEolStyle _eolStyle;
	public:
		SvnExportArgs()
		{
			_revision = SvnRevision::Head;
		}

		property bool NotRecursive
		{
			bool get()
			{
				return _notRecursive;
			}
			void set(bool value)
			{
				_notRecursive = value;
			}
		}

		property bool IgnoreExternals
		{
			bool get()
			{
				return _ignoreExternals;
			}
			void set(bool value)
			{
				_ignoreExternals = value;
			}
		}

		property bool Overwrite
		{
			bool get()
			{
				return _overwrite;
			}
			void set(bool value)
			{
				_overwrite = value;
			}
		}

		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
			void set(SvnRevision^ value)
			{
				if(value)
					_revision = value;
				else
					_revision = SvnRevision::Head;
			}
		}

		property SvnEolStyle EolStyle
		{
			SvnEolStyle get()
			{
				return _eolStyle;
			}
			void set(SvnEolStyle value)
			{
				_eolStyle = value;
			}
		}
	};

	public ref class SvnSwitchArgs : public SvnClientArgs
	{
		bool _notRecursive;

	public:
		SvnSwitchArgs()
		{
		}

		property bool NotRecursive
		{
			bool get()
			{
				return _notRecursive;
			}
			void set(bool value)
			{
				_notRecursive = value;
			}
		}
	};

	public ref class SvnAddArgs : public SvnClientArgs
	{
		bool _notRecursive;
		bool _noIgnore;
		bool _force;

	public:
		SvnAddArgs()
		{
		}

		property bool NotRecursive
		{
			bool get()
			{
				return _notRecursive;
			}
			void set(bool value)
			{
				_notRecursive = value;
			}
		}

		property bool NoIgnore
		{
			bool get()
			{
				return _noIgnore;
			}
			void set(bool value)
			{
				_noIgnore = value;
			}
		}

		/// <summary>If force is not set and path is already under version control, return the error 
		/// SVN_ERR_ENTRY_EXISTS. If force is set, do not error on already-versioned items. When used 
		/// on a directory in conjunction with the recursive flag, this has the effect of scheduling 
		/// for addition unversioned files and directories scattered deep within a versioned tree.</summary>
		property bool Force
		{
			bool get()
			{
				return _force;
			}
			void set(bool value)
			{
				_force = value;
			}
		}
	};

	public ref class SvnCommitArgs : public SvnClientArgsWithCommit
	{
		bool _notRecursive;
		bool _keepLocks;
		String^ _message;
	public:
		SvnCommitArgs()
		{
		}

	public:
		property bool NotRecursive
		{
			bool get()
			{
				return _notRecursive;
			}
			void set(bool value)
			{
				_notRecursive = value;
			}
		}

		property bool KeepLocks
		{
			bool get()
			{
				return _keepLocks;
			}
			void set(bool value)
			{
				_keepLocks = value;
			}
		}
	};

	public ref class SvnStatusArgs : public SvnClientArgs
	{
		bool _notRecursive;
		bool _getAll;
		bool _update;
		bool _noIgnore;
		bool _ignoreExternals;
		SvnRevision^ _revision;

	protected public:
		event EventHandler<SvnStatusEventArgs^>^ Status;

		virtual void OnStatus(SvnStatusEventArgs^ e)
		{
			Status(this, e);
		}
	public:
		SvnStatusArgs()
		{
			_revision = SvnRevision::None;
		}

		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
			void set(SvnRevision^ value)
			{
				if(value)
					_revision = value;
				else
					_revision = SvnRevision::None;
			}
		}

		property bool NotRecursive
		{
			bool get()
			{
				return _notRecursive;
			}
			void set(bool value)
			{
				_notRecursive = value;
			}
		}

		property bool GetAll
		{
			bool get()
			{
				return _getAll;
			}
			void set(bool value)
			{
				_getAll = value;
			}
		}

		property bool Update
		{
			bool get()
			{
				return _update;
			}
			void set(bool value)
			{
				_update = value;
			}
		}

		property bool NoIgnore
		{
			bool get()
			{
				return _noIgnore;
			}
			void set(bool value)
			{
				_noIgnore = value;
			}
		}

		property bool IgnoreExternals
		{
			bool get()
			{
				return _ignoreExternals;
			}
			void set(bool value)
			{
				_ignoreExternals = value;
			}
		}
	};

	public ref class SvnLogArgs : public SvnClientArgs
	{
		SvnRevision^ _start;
		SvnRevision^ _end;
		int _limit;
		bool _logChangedPaths;
		bool _strictHistory;

	protected public:
		event EventHandler<SvnLogEventArgs^>^ Log;

		virtual void OnLog(SvnLogEventArgs^ e)
		{
			Log(this, e);
		}
	public:
		SvnLogArgs()
		{
			_start = SvnRevision::Head;
			_end = SvnRevision::Zero;
			_limit = 0;
			_logChangedPaths = true;
			_strictHistory = false;
		}

		property SvnRevision^ Start
		{
			SvnRevision^ get()
			{
				return _start;
			}
			void set(SvnRevision^ value)
			{
				if(value)
					_start = value;
				else
					_start = SvnRevision::Head;
			}
		}

		property SvnRevision^ End
		{
			SvnRevision^ get()
			{
				return _end;
			}
			void set(SvnRevision^ value)
			{
				if(value)
					_end = value;
				else
					_end = SvnRevision::Zero;
			}
		}

		property int Limit
		{
			int get()
			{
				return _limit;
			}

			void set(int value)
			{
				_limit = (value >= 0) ? value : 0;
			}
		}

		property bool LogChangedPaths
		{
			bool get()
			{
				return _logChangedPaths;
			}
			void set(bool value)
			{
				_logChangedPaths = value;
			}
		}

		property bool StrictHistory
		{
			bool get()
			{
				return _strictHistory;
			}
			void set(bool value)
			{
				_strictHistory = value;
			}
		}
	};

	public ref class SvnInfoArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
		bool _recursive;

	protected public:
		event EventHandler<SvnInfoEventArgs^>^ Info;

		virtual void OnInfo(SvnInfoEventArgs^ e)
		{
			Info(this, e);
		}
	public:
		SvnInfoArgs()
		{
			_revision = SvnRevision::None;
		}

		property bool Recursive
		{
			bool get()
			{
				return _recursive;
			}
			void set(bool value)
			{
				_recursive = value;
			}
		}

		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
			void set(SvnRevision^ value)
			{
				if(value)
					_revision = value;
				else
					_revision = SvnRevision::None;
			}
		}
	};

	public ref class SvnMkDirArgs : public SvnClientArgsWithCommit
	{
	public:
		SvnMkDirArgs()
		{}
	};

	public ref class SvnDeleteArgs : public SvnClientArgsWithCommit
	{
		bool _force;
	public:
		SvnDeleteArgs()
		{}

		/// <summary>If Force is not set then this operation will fail if any path contains locally modified 
		/// and/or unversioned items. If Force is set such items will be deleted.</summary>
		property bool Force
		{
			bool get()
			{
				return _force;
			}
			void set(bool value)
			{
				_force = value;
			}
		}
	};

	public ref class SvnImportArgs : public SvnClientArgsWithCommit
	{
		bool _notRecursive;
		bool _noIgnore;
	public:
		SvnImportArgs()
		{}

		property bool NotRecursive
		{
			bool get()
			{
				return _notRecursive;
			}
			void set(bool value)
			{
				_notRecursive = value;
			}
		}

		property bool NoIgnore
		{
			bool get()
			{
				return _noIgnore;
			}
			void set(bool value)
			{
				_noIgnore = value;
			}
		}
	};

	public ref class SvnListArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
		bool _recursive;
		SvnDirEntryItems _entryItems;
		bool _fetchLocks;

	protected public:
		event EventHandler<SvnListEventArgs^>^ List;

		virtual void OnList(SvnListEventArgs^ e)
		{
			List(this, e);
		}
	public:
		SvnListArgs()
		{
			_revision = SvnRevision::None;
			_entryItems = SvnDirEntryItems::AllFields;
		}

		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
			void set(SvnRevision^ value)
			{
				if(value)
					_revision = value;
				else
					_revision = SvnRevision::None;
			}
		}

		property SvnDirEntryItems EntryItems
		{
			SvnDirEntryItems get()
			{
				return _entryItems;
			}

			void set(SvnDirEntryItems value)
			{
				_entryItems = value;
			}
		}

		property bool FetchLocks
		{
			bool get()
			{
				return _fetchLocks;
			}
			void set(bool value)
			{
				_fetchLocks = value;
			}
		}

		property bool Recursive
		{
			bool get()
			{
				return _recursive;
			}
			void set(bool value)
			{
				_recursive = value;
			}
		}
	};

	public ref class SvnRevertArgs : public SvnClientArgs
	{
		bool _recursive;

	public:
		SvnRevertArgs()
		{
		}

		property bool Recursive
		{
			bool get()
			{
				return _recursive;
			}
			void set(bool value)
			{
				_recursive = value;
			}
		}
	};

	public ref class SvnResolvedArgs : public SvnClientArgs
	{
		bool _recursive;

	public:
		SvnResolvedArgs()
		{
		}

		property bool Recursive
		{
			bool get()
			{
				return _recursive;
			}
			void set(bool value)
			{
				_recursive = value;
			}
		}
	};

	public ref class SvnCopyArgs : public SvnClientArgsWithCommit
	{
	public:
		SvnCopyArgs()
		{
		}
	};

	public ref class SvnMoveArgs : public SvnClientArgs
	{
		bool _force;
	public:
		SvnMoveArgs()
		{
		}

		property bool Force
		{
			bool get()
			{
				return _force;
			}
			void set(bool value)
			{
				_force = value;
			}
		}
	};

	public ref class SvnSetPropertyArgs : public SvnClientArgs
	{
		bool _recursive;
		bool _skipChecks;
	public:
		SvnSetPropertyArgs()
		{
		}

		property bool Recursive
		{
			bool get()
			{
				return _recursive;
			}
			void set(bool value)
			{
				_recursive = value;
			}
		}

		property bool SkipChecks
		{
			bool get()
			{
				return _skipChecks;
			}
			void set(bool value)
			{
				_skipChecks = value;
			}
		}
	};

	public ref class SvnGetPropertyArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
		bool _recursive;
	public:
		SvnGetPropertyArgs()
		{
			_revision = SvnRevision::None;
		}

		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
			void set(SvnRevision^ value)
			{
				if(value)
					_revision = value;
				else
					_revision = SvnRevision::None;
			}
		}

		property bool Recursive
		{
			bool get()
			{
				return _recursive;
			}
			void set(bool value)
			{
				_recursive = value;
			}
		}
	};

	public ref class SvnLockArgs : public SvnClientArgs
	{
		String^ _comment;
		bool _stealLock;

	public:
		SvnLockArgs()
		{
		}

		property String^ Comment
		{
			String^ get()
			{
				return _comment ? _comment : "";
			}

			void set(String^ value)
			{
				_comment = value;
			}
		}

		property bool StealLock
		{
			bool get()
			{
				return _stealLock;
			}
			void set(bool value)
			{
				_stealLock = value;
			}
		}
	};

	public ref class SvnUnlockArgs : public SvnClientArgs
	{
		bool _breakLock;

	public:
		SvnUnlockArgs()
		{
		}

		property bool BreakLock
		{
			bool get()
			{
				return _breakLock;
			}
			void set(bool value)
			{
				_breakLock = value;
			}
		}
	};
}
