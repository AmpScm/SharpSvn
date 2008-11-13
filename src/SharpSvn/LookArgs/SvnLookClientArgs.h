// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {

	ref class SvnLookClient;

	/// <summary>Base Extended Parameter container of <see cref="SvnLookClient" /> commands</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnLookClientArgs abstract : public SvnClientArgs
	{
	protected:
		SvnLookClientArgs()
		{
		}
	};
}
