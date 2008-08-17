// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnLookClientArgs.h"

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnLookClient" />'s Property method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnLookPropertyArgs : public SvnLookClientArgs
	{
	public:
		SvnLookPropertyArgs()
		{
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Unknown;
			}
		}
	};
}
