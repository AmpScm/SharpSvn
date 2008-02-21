// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

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
				if (!_changelists)
					_changelists = gcnew SvnChangeListCollection();
				return _changelists;
			}
		}
	};


}
