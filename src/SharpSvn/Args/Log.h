// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::Log(Uri^, SvnLogArgs^, EventHandler{SvnLogEventArgs^}^ logHandler)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnLogArgs : public SvnClientArgs
	{
		SvnRevision^ _pegRevision;
		SvnRevision^ _start;
		SvnRevision^ _end;
		int _limit;
		bool _noLogChangedPaths;
		bool _strictNodeHistory;
		bool _includeMerged;
		bool _ommitMessages;
		Uri^ _baseUri;
		Collection<String^>^ _retrieveProperties;

	internal:
		int _mergeLogLevel; // Used by log handler to provide mergeLogLevel
		Uri^ _searchRoot;

	public:
		event EventHandler<SvnLogEventArgs^>^ Log;

	protected public:
		virtual void OnLog(SvnLogEventArgs^ e)
		{
			Log(this, e);
		}

	public:
		/// <summary>Initializes a new <see cref="SvnLogArgs" /> instance with default properties</summary>
		SvnLogArgs()
		{
			_start = SvnRevision::Head;
			_end = SvnRevision::Zero;
			_pegRevision = SvnRevision::None;
			//_limit = 0;
			//_noLogChangedPaths = false;
			//_strictHistory = false;
		}

		/// <summary>Initializes a new <see cref="SvnLogArgs" /> instance with the specified range</summary>
		SvnLogArgs(SvnRevisionRange^ range)
		{
			if(!range)
				throw gcnew ArgumentNullException("range");

			Range = range;
			_pegRevision = SvnRevision::None;
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Log;
			}
		}

		/// <summary>Gets the revision in which the Url's are evaluated (Aka peg revision)</summary>
		property SvnRevision^ OriginRevision
		{
			SvnRevision^ get()
			{
				return _pegRevision;
			}
			void set(SvnRevision^ value)
			{
				if (value)
					_pegRevision = value;
				else
					_pegRevision = SvnRevision::None;
			}
		}


		/// <summary>Gets or sets the log range as <see cref="SvnRevisionRange" /></summary>
		property SvnRevisionRange^ Range
		{
			SvnRevisionRange^ get()
			{
				return gcnew SvnRevisionRange(Start, End);
			}

			void set(SvnRevisionRange^ value)
			{
				if(!value)
				{
					Start = nullptr;
					End = nullptr;
				}
				else
				{
					Start = value->StartRevision;
					End = value->EndRevision;
				}
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
				if (value)
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
				if (value)
					_end = value;
				else
					_end = SvnRevision::None;
			}
		}

		/// <summary>Gets or sets the limit on the number of log items fetched</summary>
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

		/// <summary>Gets or sets a boolean indicating whether the paths changed in the revision should be provided</summary>
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

		/// <summary>Gets or sets a boolean indicating whether only the history of this exact node should be fetched (Aka stop on copying)</summary>
		/// <remarks>If @a StrictNodeHistory is set, copy history (if any exists) will
		/// not be traversed while harvesting revision logs for each target. </remarks>
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

		/// <summary>Gets or sets a boolean indicating whether the merged revisions should be fetched instead of the node changes</summary>
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

		/// <summary>Gets or sets a boolean indicating whether the logmessage should be fetched</summary>
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

		/// <summary>Gets or sets the base uri to which relative Uri's are relative</summary>
		property Uri^ BaseUri
		{
			Uri^ get()
			{
				return _baseUri;
			}
			void set(Uri^ value)
			{
				_baseUri = value;
			}
		}

		/// <summary>Gets the list of properties to retrieve. Only SVN 1.5+ repositories allow adding custom properties to this list</summary>
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

}
