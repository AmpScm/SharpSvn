// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::Blame(SvnTarget^,SvnBlameArgs^,EventHandler{SvnBlameEventArgs^}^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnBlameArgs : public SvnClientArgs
	{
		SvnRevision ^_start;
		SvnRevision ^_end;
		SvnIgnoreSpacing _ignoreSpace;
		bool _ignoreEol;
		bool _ignoreMime;
		bool _includeMergedRevisions;

	public:
		SvnBlameArgs()
		{
			_start = SvnRevision::None;
			_end = SvnRevision::None;
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

		/// <summary>Gets or sets the blame range as <see cref="SvnRevisionRange" /></summary>
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


}
