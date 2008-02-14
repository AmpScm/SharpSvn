// $Id: SvnClientArgs_Wc.h 266 2008-02-14 13:24:29Z bhuijben $
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

public ref class SvnErrorEventArgs : public SvnCancelEventArgs
	{
		initonly SvnException ^_exception;
	public:
		SvnErrorEventArgs(SvnException ^exception)
		{
			if (!exception)
				throw gcnew ArgumentNullException("exception");

			_exception = exception;
		}

		property SvnException^ Exception
		{
			SvnException^ get()
			{
				return _exception;
			}
		}
	};


}