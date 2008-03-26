// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	public ref class SvnGetWorkingCopyStateArgs : public SvnClientArgs
	{
		bool _getBaseFile;
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
		SvnGetWorkingCopyStateArgs()
		{
		}

		property bool RetrieveBaseFile
		{
			bool get()
			{
				return _getBaseFile;
			}
			void set(bool value)
			{
				_getBaseFile = value;
			}
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
