// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientEventArgs.h

namespace SharpSvn {

	public ref class SvnCancelEventArgs : public SvnEventArgs
	{
		bool _cancel;

	public:
		SvnCancelEventArgs()
		{
		}

	public:
		property bool Cancel
		{
			bool get()
			{
				return _cancel;
			}
			void set(bool value)
			{
				_cancel = value;
			}
		}
	};
}