// $Id: SvnClientEventArgs_Global.h 234 2008-02-07 15:26:14Z bhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {

	public ref class SvnWorkingCopyState sealed
	{
		bool _isText;
		String^ _pristineFile;
	internal:
		SvnWorkingCopyState(bool isText, String^ pristineFile)
		{
			_isText = isText;
			_pristineFile = pristineFile;
		}

	public:
		property String^ WorkingCopyBasePath
		{
			String^ get()
			{
				return _pristineFile;
			}
		}

		property bool IsTextFile
		{
			bool get()
			{
				return _isText;
			}
		}
	};
}