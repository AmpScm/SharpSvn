// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	public ref class SvnGetWorkingCopyStateArgs : public SvnClientArgs
	{
		bool _dontGetBaseFile;
		bool _dontGetFileData;

	public:
		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::GetWorkingCopyInfo;
			}
		}

	public:
		SvnGetWorkingCopyStateArgs()
		{
		}

		property bool GetBaseFile
		{
			bool get()
			{
				return !_dontGetBaseFile;
			}
			void set(bool value)
			{
				_dontGetBaseFile = !value;
			}
		}

		property bool GetFileData
		{
			bool get()
			{
				return !_dontGetFileData;
			}
			void set(bool value)
			{
				_dontGetFileData = !value;
			}
		}

	};

}
