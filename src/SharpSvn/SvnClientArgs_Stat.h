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
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient::DiffSummary(SvnTarget^,SvnTarget^,SvnDiffSummaryArgs^,EventHandler{SvnDiffSummaryEventArgs^}^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
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
	};

	/// <summary>Extended Parameter container of <see cref="SvnClient" />'s GetAppliedMergeInfo method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnGetAppliedMergeInfoArgs : public SvnClientArgs
	{
	public:
		SvnGetAppliedMergeInfoArgs()
		{
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
	};

};
