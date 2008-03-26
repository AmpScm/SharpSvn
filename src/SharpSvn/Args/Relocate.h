// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::Relocate(String^, Uri^, Uri^, SvnRelocateArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnRelocateArgs : public SvnClientArgs
	{
		bool _nonRecursive;
	public:
		SvnRelocateArgs()
		{
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Relocate;
			}
		}

		property bool NonRecursive
		{
			bool get()
			{
				return _nonRecursive;
			}
			void set(bool value)
			{
				_nonRecursive = value;
			}
		}
	};
}