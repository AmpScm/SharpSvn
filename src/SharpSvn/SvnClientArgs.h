
#pragma once

namespace SharpSvn {
	ref class SvnException;

	public ref class SvnClientArgs abstract
	{
		bool _noThrowOnError;
		SvnException^ _exception;

	public:
		event EventHandler<SvnCancelEventArgs^>^	Cancel;
		event EventHandler<SvnProgressEventArgs^>^	Progress;
		event EventHandler<SvnNotifyEventArgs^>^	Notify;

	protected public:
		SvnClientArgs()
		{
		}

		virtual void OnCancel(SvnCancelEventArgs^ e)
		{
			Cancel(this, e);
		}

		virtual void OnProgress(SvnProgressEventArgs^ e)
		{
			Progress(this, e);
		}

		virtual void OnNotify(SvnNotifyEventArgs^ e)
		{
			Notify(this, e);
		}

		virtual void OnSvnError(SvnErrorEventArgs^ e)
		{
			UNUSED_ALWAYS(e);
		}

	public:
		/// <summary>
		/// Gets or sets a boolean indicating whether the call must throw an error if an exception occurs.
		/// If an exception would occur, the method returns false and the <see cref="Exception" /> property
		/// is set to the exception which would have been throw.
		/// </summary>
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

		property bool InvokationCeased
		{
			bool get();
		}

	internal:
		[System::Diagnostics::DebuggerStepThroughAttribute()]
		bool HandleResult(SvnClientContext^ client, svn_error_t *error);
	};

	public ref class SvnClientArgsWithCommit : public SvnClientArgs
	{
		String^ _logMessage;

	public:
		event EventHandler<SvnCommittingEventArgs^>^ Committing;

	protected:
		SvnClientArgsWithCommit()
		{
		}

	protected public:
		virtual void OnCommitting(SvnCommittingEventArgs^ e)
		{
			if(LogMessage && !e->LogMessage)
				e->LogMessage = LogMessage;

			Committing(this, e);
		}

	public:
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
	};

	public ref class SvnClientArgsWithConflict : public SvnClientArgs
	{
		String^ _logMessage;

	public:
		event EventHandler<SvnConflictEventArgs^>^ Conflict;

	protected:
		SvnClientArgsWithConflict()
		{
		}

	protected public:
		virtual void OnConflict(SvnConflictEventArgs^ e)
		{
			Conflict(this, e);
		}

	public:
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
			_depth = SvnDepth::Unknown;
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
				_depth = EnumVerifier::Verify(value);
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

	public ref class SvnUpdateArgs : public SvnClientArgsWithConflict
	{
		SvnDepth _depth;
		bool _ignoreExternals;
		bool _allowUnversionedObstructions;
		SvnRevision^ _revision;
	public:
		SvnUpdateArgs()
		{
			_depth = SvnDepth::Unknown;
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
				_depth = EnumVerifier::Verify(value);
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

	public ref class SvnExportArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		bool _ignoreExternals;
		bool _overwrite;
		SvnRevision^ _revision;
		SvnLineStyle _lineStyle;
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
				_depth = EnumVerifier::Verify(value);
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

		property SvnLineStyle LineStyle
		{
			SvnLineStyle get()
			{
				return _lineStyle;
			}
			void set(SvnLineStyle value)
			{
				_lineStyle = EnumVerifier::Verify(value);
			}
		}
	};

	public ref class SvnSwitchArgs : public SvnClientArgsWithConflict
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
				_depth = EnumVerifier::Verify(value);
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

	public ref class SvnRelocateArgs : public SvnClientArgs
	{
		bool _nonRecursive;
	public:
		SvnRelocateArgs()
		{
		}

		property bool NonRecursive
		{
			bool get()
			{
				return _nonRecursive;
			}
			void set(bool value)
			{
				_nonRecursive = value;
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
				_depth = EnumVerifier::Verify(value);
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
				_depth = EnumVerifier::Verify(value);
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
				_depth = EnumVerifier::Verify(value);
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
		bool _noLogChangedPaths;
		bool _strictNodeHistory;
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
			//_limit = 0;
			//_noLogChangedPaths = false;
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
				return !_noLogChangedPaths;
			}
			void set(bool value)
			{
				_noLogChangedPaths = !value;
			}
		}

		property bool StrictNodeHistory
		{
			bool get()
			{
				return _strictNodeHistory;
			}
			void set(bool value)
			{
				_strictNodeHistory = value;
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
				_depth = EnumVerifier::Verify(value);
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

	public ref class SvnCreateDirectoryArgs : public SvnClientArgsWithCommit
	{
		bool _makeParents;
	public:
		SvnCreateDirectoryArgs()
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
		bool _noIgnore;
		bool _ignoreUnknownNodeTypes;
		SvnDepth _depth;
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
				_depth = EnumVerifier::Verify(value);
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
			_entryItems = SvnDirEntryItems::SnvListDefault;
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = EnumVerifier::Verify(value);
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
				_depth = EnumVerifier::Verify(value);
			}
		}
	};

	public ref class SvnResolvedArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		SvnAccept _which;

	public:
		SvnResolvedArgs()
		{
			_depth = SvnDepth::Empty;
			_which = SvnAccept::None;
		}

		SvnResolvedArgs(SvnAccept which)
		{
			_depth = SvnDepth::Empty;
			_which = which;
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = EnumVerifier::Verify(value);
			}
		}

		property SvnAccept Which
		{
			SvnAccept get()
			{
				return _which;
			}
			void set(SvnAccept value)
			{
				_which = EnumVerifier::Verify(value);
			}
		}
	};

	public ref class SvnCopyArgs : public SvnClientArgsWithCommit
	{
		bool _makeParents;
		bool _alwaysCopyBelow;
		bool _withoutMergeHistory;

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

		property bool WithoutMergeHistory
		{
			bool get()
			{
				return _withoutMergeHistory;
			}
			void set(bool value)
			{
				_withoutMergeHistory = value;
			}
		}
	};

	public ref class SvnMoveArgs : public SvnClientArgs
	{
		bool _force;
		bool _moveAsChild;
		bool _makeParents;
		bool _withoutMergeHistory;
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

		property bool WithoutMergeHistory
		{
			bool get()
			{
				return _withoutMergeHistory;
			}
			void set(bool value)
			{
				_withoutMergeHistory = value;
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
				_depth = EnumVerifier::Verify(value);
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
				_depth = EnumVerifier::Verify(value);
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
				_depth = EnumVerifier::Verify(value);
			}
		}
	};

	public ref class SvnSetRevisionPropertyArgs : public SvnClientArgs
	{
		bool _force;
	public:
		SvnSetRevisionPropertyArgs()
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

	public ref class SvnGetRevisionPropertyArgs : public SvnClientArgs
	{
	public:
		SvnGetRevisionPropertyArgs()
		{
		}		
	};

	public ref class SvnRevisionPropertyListArgs : public SvnClientArgs
	{
	/*public:
		event EventHandler<SvnRevisionPropertyListEventArgs^>^ PropertyList;

	protected public:
		virtual void OnPropertyList(SvnRevisionPropertyListEventArgs^ e)
		{
			PropertyList(this, e);
		}*/

	public:
		SvnRevisionPropertyListArgs()
		{
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

	public ref class SvnWriteArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
	public:
		SvnWriteArgs()
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

	public ref class SvnMergeArgs : public SvnClientArgsWithConflict
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
			_depth = SvnDepth::Unknown;
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
			void set(SvnDepth value)
			{
				_depth = EnumVerifier::Verify(value);
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
				_depth = EnumVerifier::Verify(value);
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

	public ref class SvnDiffSummaryArgs : public SvnClientArgs
	{
		bool _noticeAncestry;
		SvnDepth _depth;
	public:
		SvnDiffSummaryArgs()
		{
			_depth = SvnDepth::Infinity;
		}

	public:
		event EventHandler<SvnDiffSummaryEventArgs^>^ SummaryHandler;

	protected public:
		virtual void OnSummaryHandler(SvnDiffSummaryEventArgs^ e)
		{
			SummaryHandler(this, e);
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
				_depth = EnumVerifier::Verify(value);
			}
		}

		property bool IgnoreAncestry
		{
			bool get()
			{
				return !_noticeAncestry;
			}
			void set(bool value)
			{
				_noticeAncestry = !value;
			}
		}
	};

	public ref class SvnBlameArgs : public SvnClientArgs
	{
		SvnRevision ^_from;
		SvnRevision ^_to;
		SvnIgnoreSpacing _ignoreSpace;
		bool _ignoreEol;
		bool _ignoreMime;
		bool _includeMergedRevisions;

	public:
		SvnBlameArgs()
		{
			_from = SvnRevision::Zero;
			_to = SvnRevision::Head;
		}

	public:
		event EventHandler<SvnBlameEventArgs^>^ BlameHandler;

	protected public:
		virtual void OnBlameHandler(SvnBlameEventArgs^ e)
		{
			BlameHandler(this, e);
		}

	public:
		property SvnRevision^ Start
		{
			SvnRevision^ get()
			{
				return _from;
			}
			void set(SvnRevision^ value)
			{
				if(!value)
					throw gcnew ArgumentNullException("value");

				_from = value;
			}
		}

		property SvnRevision^ End
		{
			SvnRevision^ get()
			{
				return _to;
			}
			void set(SvnRevision^ value)
			{
				if(!value)
					throw gcnew ArgumentNullException("value");

				_to = value;
			}
		}

		property SvnIgnoreSpacing IgnoreSpacing
		{
			SvnIgnoreSpacing get()
			{
				return _ignoreSpace;
			}
			void set(SvnIgnoreSpacing value)
			{
				_ignoreSpace = value;
			}
		}

		property bool IgnoreLineEndings
		{
			bool get()
			{
				return _ignoreEol;
			}
			void set(bool value)
			{
				_ignoreEol = value;
			}
		}

		property bool IgnoreMimeType
		{
			bool get()
			{
				return _ignoreMime;
			}
			void set(bool value)
			{
				_ignoreMime = value;
			}
		}

		property bool IncludeMergedRevisions
		{
			bool get()
			{
				return _includeMergedRevisions;
			}
			void set(bool value)
			{
				_includeMergedRevisions = value;
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

	public ref class SvnGetSuggestedMergeSourcesArgs : public SvnClientArgs
	{
	public:
		SvnGetSuggestedMergeSourcesArgs()
		{
		}
	};

	public ref class SvnGetAppliedMergeInfoArgs : public SvnClientArgs
	{
	public:
		SvnGetAppliedMergeInfoArgs()
		{
		}
	};

	public ref class SvnGetAvailableMergeInfoArgs : public SvnClientArgs
	{
	public:
		SvnGetAvailableMergeInfoArgs()
		{
		}
	};

	public ref class SvnCreateRepositoryArgs : public SvnClientArgs
	{
		bool _bdbNoFSyncCommit;
		bool _bdbKeepLogs;
		SvnRepositoryFilesystem _reposType;
		SvnRepositoryCompatibility _reposCompat;

	public:
		SvnCreateRepositoryArgs()
		{
		}


		property bool BerkeleyDBNoFSyncAtCommit
		{
			bool get()
			{
				return _bdbNoFSyncCommit;
			}
			void set(bool value)
			{
				_bdbNoFSyncCommit = value;
			}
		}

		property bool BerkeleyDBKeepTransactionLogs
		{
			bool get()
			{
				return _bdbKeepLogs;
			}
			void set(bool value)
			{
				_bdbKeepLogs = value;
			}
		}

		property SvnRepositoryFilesystem RepositoryType
		{
			SvnRepositoryFilesystem get()
			{
				return _reposType;
			}
			void set(SvnRepositoryFilesystem value)
			{
				_reposType = EnumVerifier::Verify(value);
			}
		}

		property SvnRepositoryCompatibility RepositoryCompatibility
		{
			SvnRepositoryCompatibility get()
			{
				return _reposCompat;
			}
			void set(SvnRepositoryCompatibility value)
			{
				_reposCompat = EnumVerifier::Verify(value);
			}
		}
	};

	public ref class SvnDeleteRepositoryArgs : public SvnClientArgs
	{
	public:
		SvnDeleteRepositoryArgs()
		{
		}
	};
	public ref class SvnRecoverRepositoryArgs : public SvnClientArgs
	{
		bool _nonBlocking;
	public:
		SvnRecoverRepositoryArgs()
		{
		}

		property bool NonBlocking
		{
			bool get()
			{
				return _nonBlocking;
			}
			void set(bool value)
			{
				_nonBlocking = value;
			}
		}
	};
}
