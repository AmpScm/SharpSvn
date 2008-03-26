// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

	public ref class SvnProcessingEventArgs sealed : public SvnEventArgs
	{
		initonly SvnCommandType _commandType;

	internal:
		SvnProcessingEventArgs(SvnCommandType commandType)
		{
			EnumVerifier::Verify(commandType);

			_commandType = commandType;
		}

	public:
		property SvnCommandType CommandType
		{
			SvnCommandType get()
			{
				return _commandType;
			}
		}

		/// <summary>Serves as a hashcode for the specified type</summary>
		virtual int GetHashCode() override
		{
			return CommandType.GetHashCode();
		}
	};

}