// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

public ref class SvnProcessingEventArgs sealed : public SvnEventArgs
	{
		initonly SvnClientCommandType _commandType;

	internal:
		SvnProcessingEventArgs(SvnClientCommandType commandType)
		{
			EnumVerifier::Verify(commandType);

			_commandType = commandType;
		}

	internal:
		property SvnClientCommandType CommandType
		{
			SvnClientCommandType get()
			{
				return _commandType;
			}
		}
	};

}