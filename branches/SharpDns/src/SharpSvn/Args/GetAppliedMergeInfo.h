// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

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

}
