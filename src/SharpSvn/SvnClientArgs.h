
#pragma once

namespace SharpSvn {
	ref class SvnException;

	public ref class SvnClientArgs abstract
	{		
		bool _noThrowOnError;
		SvnException^ _exception;

	public:
		event EventHandler<SvnClientCancelEventArgs^>^		Cancel;
		event EventHandler<SvnClientProgressEventArgs^>^	Progress;
		event EventHandler<SvnClientNotifyEventArgs^>^		Notify;

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

		internal:
			void set(SvnException^ value)
			{
				_exception = value;
			}
		}

	internal:
		bool HandleResult(svn_error_t *error);
	};

	public ref class SvnClientArgsWithCommit : public SvnClientArgs
	{
		String^ _commitMessage;

	public:
		event EventHandler<SvnClientBeforeCommitEventArgs^>^ BeforeCommit;

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
		SvnDepth _depth;
		bool _ignoreExternals;
		SvnRevision^ _revision;
		bool _allowUnversionedObstructions;

	public:
		SvnCheckOutArgs()
		{
			_depth = SvnDepth::Infinity;
			_revision = SvnRevision::Head;
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = value;
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
		SvnDepth _depth;
		bool _ignoreExternals;
		bool _allowUnversionedObstructions;
		SvnRevision^ _revision;
	public:
		SvnUpdateArgs()
		{
			_depth = SvnDepth::Infinity;
			_revision = SvnRevision::Head;
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = value;
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
		SvnDepth _depth;
		bool _ignoreExternals;
		bool _overwrite;
		SvnRevision^ _revision;
		SvnEolStyle _eolStyle;
	public:
		SvnExportArgs()
		{
			_depth = SvnDepth::Infinity;
			_revision = SvnRevision::Head;
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = value;
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
		SvnDepth _depth;
		bool _allowUnversionedObstructions;
		bool _ignoreExternals;

	public:
		SvnSwitchArgs()
		{
			_depth = SvnDepth::Infinity;
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = value;
			}
		}

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

	public ref class SvnAddArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		bool _noIgnore;
		bool _force;
		bool _addParents;

	public:
		SvnAddArgs()
		{
			_depth = SvnDepth::Infinity;
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = value;
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

		/// <summary>
		///
		/// </summary>
		property bool AddParents
		{
			bool get()
			{
				return _addParents;
			}
			void set(bool value)
			{
				_addParents = value;
			}
		}
	};

	public ref class SvnCommitArgs : public SvnClientArgsWithCommit
	{
		SvnDepth _depth;
		bool _keepLocks;
		bool _keepChangelist;
		String^ _message;
		String^ _changelist;
	public:
		SvnCommitArgs()
		{
			_depth = SvnDepth::Infinity;
		}

	public:
		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = value;
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

		property String^ Changelist
		{
			String^ get()
			{
				return _changelist;
			}
			void set(String^ value)
			{
				_changelist = value;
			}
		}

		property bool KeepChangelist
		{
			bool get()
			{
				return _keepChangelist;
			}
			void set(bool value)
			{
				_keepChangelist = value;
			}
		}
	};

	public ref class SvnStatusArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		bool _getAll;
		bool _update;
		bool _noIgnore;
		bool _ignoreExternals;
		SvnRevision^ _revision;

	public:
		event EventHandler<SvnStatusEventArgs^>^ Status;

	protected public:		
		virtual void OnStatus(SvnStatusEventArgs^ e)
		{
			Status(this, e);
		}

	public:
		SvnStatusArgs()
		{
			_depth = SvnDepth::Infinity;
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

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = value;
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
		bool _includeMerged;
		bool _ommitMessages;

	public:
		event EventHandler<SvnLogEventArgs^>^ Log;

	protected public:
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
			//_strictHistory = false;
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

		property bool IncludeMergedRevisions
		{
			bool get()
			{
				return _includeMerged;
			}
			void set(bool value)
			{
				_includeMerged = value;
			}
		}

		property bool OmitMessages
		{
			bool get()
			{
				return _ommitMessages;
			}
			void set(bool value)
			{
				_ommitMessages = value;
			}
		}
	};

	public ref class SvnInfoArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
		SvnDepth _depth;

	public:
		event EventHandler<SvnInfoEventArgs^>^ Info;

	protected public:
		virtual void OnInfo(SvnInfoEventArgs^ e)
		{
			Info(this, e);
		}

	public:
		SvnInfoArgs()
		{
			_revision = SvnRevision::None;
			_depth = SvnDepth::Empty;
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = value;
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
		bool _makeParents;
	public:
		SvnMkDirArgs()
		{}

		property bool MakeParents
		{
			bool get()
			{
				return _makeParents;
			}

			void set(bool value)
			{
				_makeParents = value;
			}
		}
	};

	public ref class SvnDeleteArgs : public SvnClientArgsWithCommit
	{
		bool _force;
		bool _keepLocal;
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

		property bool KeepLocal
		{
			bool get()
			{
				return _keepLocal;
			}
			void set(bool value)
			{
				_keepLocal = value;
			}
		}
	};

	public ref class SvnImportArgs : public SvnClientArgsWithCommit
	{
		SvnDepth _depth;
		bool _noIgnore;
		bool _ignoreUnknownNodeTypes;
	public:
		SvnImportArgs()
		{
			_depth = SvnDepth::Infinity;
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = value;
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

		property bool IgnoreUnknownNodeTypes
		{
			bool get()
			{
				return _ignoreUnknownNodeTypes;
			}
			void set(bool value)
			{
				_ignoreUnknownNodeTypes = value;
			}
		}
	};

	public ref class SvnListArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
		SvnDepth _depth;
		SvnDirEntryItems _entryItems;
		bool _fetchLocks;

	public:
		event EventHandler<SvnListEventArgs^>^ List;

	protected public:
		virtual void OnList(SvnListEventArgs^ e)
		{
			List(this, e);
		}

	public:
		SvnListArgs()
		{
			_depth = SvnDepth::Files;
			_revision = SvnRevision::None;
			_entryItems = SvnDirEntryItems::AllFields;
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = value;
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
	};

	public ref class SvnRevertArgs : public SvnClientArgs
	{
		SvnDepth _depth;

	public:
		SvnRevertArgs()
		{
			_depth = SvnDepth::Empty;
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = value;
			}
		}
	};

	public ref class SvnResolvedArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		SvnAccept _accept;

	public:
		SvnResolvedArgs()
		{
			_depth = SvnDepth::Empty;
			_accept = SvnAccept::Default;
		}

		SvnResolvedArgs(SvnAccept accept)
		{
			_depth = SvnDepth::Empty;
			_accept = accept;
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = value;
			}
		}

		property SvnAccept Accept
		{
			SvnAccept get()
			{
				return _accept;
			}
			void set(SvnAccept value)
			{
				_accept = value;
			}
		}
	};

	public ref class SvnCopyArgs : public SvnClientArgsWithCommit
	{
		bool _makeParents;
		bool _alwaysCopyBelow;

	public:
		SvnCopyArgs()
		{
		}

		property bool MakeParents
		{
			bool get()
			{
				return _makeParents;
			}
			void set(bool value)
			{
				_makeParents = value;
			}
		}

		/// <summary>Always copies the result to below the target (this behaviour is always used if multiple targets are provided)</summary>
		property bool AlwaysCopyAsChild
		{
			bool get()
			{
				return _alwaysCopyBelow;
			}
			void set(bool value)
			{
				_alwaysCopyBelow = value;
			}
		}
	};

	public ref class SvnMoveArgs : public SvnClientArgs
	{
		bool _force;
		bool _moveAsChild;
		bool _makeParents;
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

		property bool AlwaysMoveAsChild
		{
			bool get()
			{
				return _moveAsChild;
			}
			void set(bool value)
			{
				_moveAsChild = value;
			}
		}

		property bool MakeParents
		{
			bool get()
			{
				return _makeParents;
			}
			void set(bool value)
			{
				_makeParents = value;
			}
		}
	};

	public ref class SvnSetPropertyArgs : public SvnClientArgsWithCommit
	{
		SvnDepth _depth;
		bool _skipChecks;
		__int64 _baseRevision;
	public:
		SvnSetPropertyArgs()
		{
			_depth = SvnDepth::Empty;
			_baseRevision = SVN_INVALID_REVNUM;
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = value;
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

		property __int64 BaseRevision
		{
			__int64 get()
			{ 
				return _baseRevision; 
			}
			void set(__int64 value)
			{
				if(value >= 0)
					_baseRevision = value;
				else 
					_baseRevision = SVN_INVALID_REVNUM;
			}
		}
	};

	public ref class SvnGetPropertyArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
		SvnDepth _depth;
	public:
		SvnGetPropertyArgs()
		{
			_depth = SvnDepth::Empty;
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

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = value;
			}
		}
	};

	public ref class SvnPropertyListArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
		SvnDepth _depth;

	public:
		event EventHandler<SvnPropertyListEventArgs^>^ PropertyList;

	protected public:
		virtual void OnPropertyList(SvnPropertyListEventArgs^ e)
		{
			PropertyList(this, e);
		}

	public:
		SvnPropertyListArgs()
		{
			_depth = SvnDepth::Empty;
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

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = value;
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

	public ref class SvnCatArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
	public:
		SvnCatArgs()
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
	};

	public ref class SvnMergeArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		bool _ignoreAncestry;
		bool _force;
		bool _recordOnly;
		bool _dryRun;
		IList<String^>^ _mergeArguments;
	public:
		SvnMergeArgs()
		{
			_depth = SvnDepth::Infinity;

		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = value;
			}
		}

		property bool IgnoreAncestry
		{
			bool get()
			{
				return _ignoreAncestry;
			}
			void set(bool value)
			{
				_ignoreAncestry = value;
			}
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

		property bool RecordOnly
		{
			bool get()
			{
				return _recordOnly;
			}
			void set(bool value)
			{
				_recordOnly = value;
			}
		}

		property bool DryRun
		{
			bool get()
			{
				return _dryRun;
			}
			void set(bool value)
			{
				_dryRun = value;
			}
		}

		property IList<String^>^ MergeArguments
		{
			IList<String^>^ get()
			{
				return _mergeArguments;
			}
			void set(IList<String^>^ value)
			{
				if(value)
					_mergeArguments = (gcnew System::Collections::Generic::List<String^>(value))->AsReadOnly();
				else
					_mergeArguments = nullptr;
			}
		}
	};

	public ref class SvnDiffArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		bool _ignoreAncestry;
		bool _noDeleted;
		bool _ignoreContentType;
		String^ _headerEncoding;
		IList<String^>^ _diffArguments;
	public:
		SvnDiffArgs()
		{
			_depth = SvnDepth::Infinity;
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = value;
			}
		}

		property bool IgnoreAncestry
		{
			bool get()
			{
				return _ignoreAncestry;
			}
			void set(bool value)
			{
				_ignoreAncestry = value;
			}
		}

		property bool NoDeleted
		{
			bool get()
			{
				return _noDeleted;
			}
			void set(bool value)
			{
				_noDeleted = value;
			}
		}

		property bool IgnoreContentType
		{
			bool get()
			{
				return _ignoreContentType;
			}
			void set(bool value)
			{
				_ignoreContentType = value;
			}
		}

		property String^ HeaderEncoding
		{
			String^ get()
			{
				return _headerEncoding ? _headerEncoding : "utf-8";
			}
			void set(String^ value)
			{
				_headerEncoding = value;
			}
		}

		property IList<String^>^ DiffArguments
		{
			IList<String^>^ get()
			{
				return _diffArguments;
			}
			void set(IList<String^>^ value)
			{
				if(value)
					_diffArguments = (gcnew System::Collections::Generic::List<String^>(value))->AsReadOnly();
				else
					_diffArguments = nullptr;
			}
		}
	};

	public ref class SvnAddToChangeListArgs : public SvnClientArgs
	{
	public:
		SvnAddToChangeListArgs()
		{
		}
	};

	public ref class SvnRemoveFromChangeListArgs : public SvnClientArgs
	{
	public:
		SvnRemoveFromChangeListArgs()
		{
		}
	};


	public ref class SvnListChangeListArgs : public SvnClientArgs
	{
	public:
		SvnListChangeListArgs()
		{
		}

	public:
		event EventHandler<SvnListChangeListEventArgs^>^ ListChangeList;

	protected public:
		virtual void OnListChangeList(SvnListChangeListEventArgs^ e)
		{
			ListChangeList(this, e);
		}
	};
}
