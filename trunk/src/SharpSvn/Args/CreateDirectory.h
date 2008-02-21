// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::CreateDirectory(String^, SvnCreateDirectoryArgs^)" /> and <see cref="SvnClient::RemoteCreateDirectory(Uri^, SvnCreateDirectoryArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnCreateDirectoryArgs : public SvnClientArgsWithCommit
	{
		bool _makeParents;
	public:
		SvnCreateDirectoryArgs()
		{}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::CreateDirectory;
			}
		}

		property bool MakeParents
		{
			bool get()
			{
				return _makeParents;
			}

			void set(bool value)
			{
				_makeParents = value;
			}
		}
	};

}
