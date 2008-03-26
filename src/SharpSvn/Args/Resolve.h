// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {
	/// <summary>Extended Parameter container of <see cref="SvnClient::Resolved(String^, SvnResolvedArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnResolveArgs : public SvnClientArgs
	{
		SvnDepth _depth;

	public:
		SvnResolveArgs()
		{
			_depth = SvnDepth::Empty;
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Resolved;
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
}