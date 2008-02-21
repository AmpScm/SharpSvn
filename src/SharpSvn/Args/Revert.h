// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::Revert(String^, SvnRevertArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnRevertArgs : public SvnClientArgs
	{
		SvnDepth _depth;
		SvnChangeListCollection^ _changelists;

	public:
		SvnRevertArgs()
		{
			_depth = SvnDepth::Empty;
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Revert;
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