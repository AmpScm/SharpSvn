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
		bool _createParents;
	public:
		SvnCreateDirectoryArgs()
		{}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::CreateDirectory;
			}
		}

		property bool CreateParents
		{
			bool get()
			{
				return _createParents;
			}

			void set(bool value)
			{
				_createParents = value;
			}
		}

		[Obsolete("Naming: Use CreateParents")]
		property bool MakeParents
		{
			bool get()
			{
				return CreateParents;
			}

			void set(bool value)
			{
				CreateParents = value;
			}
		}
	};

}
