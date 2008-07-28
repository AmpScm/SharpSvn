// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {

	public ref class SvnWorkingCopyState sealed
	{
		bool _isText;
	internal:
		SvnWorkingCopyState(bool isText)
		{
			_isText = isText;
		}

	public:
		property bool IsTextFile
		{
			bool get()
			{
				return _isText;
			}
		}
	};
}