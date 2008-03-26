// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient" />'s GetRevisionProperty</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnGetRevisionPropertyArgs : public SvnClientArgs
	{
	public:
		SvnGetRevisionPropertyArgs()
		{
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::GetRevisionProperty;
			}
		}
	};

}
