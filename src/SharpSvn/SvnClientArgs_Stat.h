// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::Status(String^, SvnStatusArgs^, EventHandler{SvnStatusEventArgs^})" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnStatusArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		bool _getAll;
		bool _update;
		bool _noIgnore;
		bool _ignoreExternals;
		SvnRevision^ _revision;
		SvnChangeListCollection^ _changelists;

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

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Status;
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

		/// <summary>Gets or sets a boolean indicating whether all status properties should be retrieved</summary>
		/// <remarks>
		/// If @a get_all is set, retrieve all entries; otherwise,
		/// retrieve only "interesting" entries (local mods and/or
		/// out of date
		/// </remarks>
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

		/// <summary>Gets or sets a boolean indicating whether the repository should be contacted to retrieve aut of date information</summary>
		/// <remarks>
		/// If Update is set, contact the repository and augment the
		/// status structures with information about out-of-dateness (with
		/// respect to @a revision).  Also, if @a result_rev is not @c NULL,
		/// set @a *result_rev to the actual revision against which the
		/// working copy was compared (result_rev is not meaningful unless
		/// update is set
		/// </remarks>
		property bool ContactRepository
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

		/// <summary>Gets or sets a boolean indicating whether externals should be ignored</summary>
		/// <remarks>
		/// If IgnoreExternals is not set, then recurse into externals
		/// definitions (if any exist) after handling the main target.  This
		/// calls the client notification function (in @a ctx) with the @c
		/// svn_wc_notify_status_external action before handling each externals
		/// definition, and with @c svn_wc_notify_status_completed
		/// after each.
		/// </remarks>
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

		/// <summary>Gets the list of changelist-names</summary>
		property SvnChangeListCollection^ ChangeLists
		{
			SvnChangeListCollection^ get()
			{
				if(!_changelists)
					_changelists = gcnew SvnChangeListCollection();
				return _changelists;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::List(SvnTarget^, SvnListArgs^, EventHandler{SvnListEventArgs^}^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
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
			_entryItems = SvnDirEntryItems::SvnListDefault;
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::List;
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

	/// <summary>Extended Parameter container of <see cref="SvnClient::Log(SvnTarget^, SvnLogArgs^, EventHandler{SvnLogEventArgs^}^ logHandler)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnLogArgs : public SvnClientArgs
	{
		SvnRevision^ _start;
		SvnRevision^ _end;
		int _limit;
		bool _noLogChangedPaths;
		bool _strictNodeHistory;
		bool _includeMerged;
		bool _ommitMessages;
		Collection<String^>^ _retrieveProperties;

	internal:
		int _mergeLogLevel; // Used by log handler to provide mergeLogLevel

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

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Log;
			}
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
					_start = SvnRevision::None;
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
					_end = SvnRevision::None;
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

		property Collection<String^>^ RetrieveProperties
		{
			Collection<String^>^ get();
		}

	internal:
		property bool RetrievePropertiesUsed
		{
			bool get()
			{
				return _retrieveProperties != nullptr;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::Info(SvnTarget^ target, SvnInfoArgs^ args, EventHandler{SvnInfoEventArgs^}^ infoHandler)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnInfoArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
		SvnDepth _depth;
		SvnChangeListCollection^ _changelists;

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

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Info;
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

		/// <summary>Gets the list of changelist-names</summary>
		property SvnChangeListCollection^ ChangeLists
		{
			SvnChangeListCollection^ get()
			{
				if(!_changelists)
					_changelists = gcnew SvnChangeListCollection();
				return _changelists;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient" />'s Diff command</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnDiffArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		bool _ignoreAncestry;
		bool _noDeleted;
		bool _ignoreContentType;
		String^ _headerEncoding;
		String^ _relativeFrom;
		SvnCommandLineArgumentCollection^ _diffArguments;
		SvnChangeListCollection^ _changelists;
	public:
		SvnDiffArgs()
		{
			_depth = SvnDepth::Infinity;
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Diff;
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

		property SvnCommandLineArgumentCollection^ DiffArguments
		{
			SvnCommandLineArgumentCollection^ get()
			{
				if(!_diffArguments)
					_diffArguments = gcnew SvnCommandLineArgumentCollection();

				return _diffArguments;
			}
		}

		property String^ RelativeToPath
		{
			String^ get()
			{
				return _relativeFrom;
			}
			void set(String^ value)
			{
				if(String::IsNullOrEmpty(value))
					_relativeFrom = nullptr;
				else
					_relativeFrom = value;
			}
		}

		/// <summary>Gets the list of changelist-names</summary>
		property SvnChangeListCollection^ ChangeLists
		{
			SvnChangeListCollection^ get()
			{
				if(!_changelists)
					_changelists = gcnew SvnChangeListCollection();
				return _changelists;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::DiffSummary(SvnTarget^,SvnTarget^,SvnDiffSummaryArgs^,EventHandler{SvnDiffSummaryEventArgs^}^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnDiffSummaryArgs : public SvnClientArgs
	{
		bool _noticeAncestry;
		SvnDepth _depth;
		SvnChangeListCollection^ _changelists;
	public:
		SvnDiffSummaryArgs()
		{
			_depth = SvnDepth::Infinity;
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::DiffSummary;
			}
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

		/// <summary>Gets the list of changelist-names</summary>
		property SvnChangeListCollection^ ChangeLists
		{
			SvnChangeListCollection^ get()
			{
				if(!_changelists)
					_changelists = gcnew SvnChangeListCollection();
				return _changelists;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::Blame(SvnTarget^,SvnBlameArgs^,EventHandler{SvnBlameEventArgs^}^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
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

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Blame;
			}
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

	/// <summary>Extended Parameter container of <see cref="SvnClient" />'s GetSuggestedMergeSources method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnGetSuggestedMergeSourcesArgs : public SvnClientArgs
	{
	public:
		SvnGetSuggestedMergeSourcesArgs()
		{
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::GetSuggestedMergeSources;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient" />'s GetAppliedMergeInfo method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnGetAppliedMergeInfoArgs : public SvnClientArgs
	{
	public:
		SvnGetAppliedMergeInfoArgs()
		{
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::GetAppliedMergeInfo;
			}
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient" />'s GetAvailableMergeInfo method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnGetAvailableMergeInfoArgs : public SvnClientArgs
	{
	public:
		SvnGetAvailableMergeInfoArgs()
		{
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::GetAvailableMergeInfo;
			}
		}
	};

	public ref class SvnGetWorkingCopyStateArgs : public SvnClientArgs
	{
		bool _dontGetBaseFile;
		bool _dontGetFileData;

	public:
		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::GetWorkingCopyInfo;
			}
		}

	public:
		SvnGetWorkingCopyStateArgs()
		{
		}

		property bool GetBaseFile
		{
			bool get()
			{
				return !_dontGetBaseFile;
			}
			void set(bool value)
			{
				_dontGetBaseFile = !value;
			}
		}

		property bool GetFileData
		{
			bool get()
			{
				return !_dontGetFileData;
			}
			void set(bool value)
			{
				_dontGetFileData = !value;
			}
		}

	};
};
