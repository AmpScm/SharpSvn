// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	public ref class SvnWorkingCopyStateArgs : public SvnClientArgs
	{
		bool _getFileData;

	public:
		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::GetWorkingCopyInfo;
			}
		}

	public:
		SvnWorkingCopyStateArgs()
		{
		}

		property bool RetrieveFileData
		{
			bool get()
			{
				return _getFileData;
			}
			void set(bool value)
			{
				_getFileData = value;
			}
		}

	};

}
