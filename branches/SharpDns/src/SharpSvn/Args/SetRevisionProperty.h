// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::SetRevisionProperty(SvnUriTarget^,String^,SvnSetRevisionPropertyArgs^,String^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnSetRevisionPropertyArgs : public SvnClientArgs
	{
		bool _force;
	public:
		SvnSetRevisionPropertyArgs()
		{
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::SetRevisionProperty;
			}
		}

		property bool Force
		{
			bool get()
			{
				return _force;
			}
			void set(bool value)
			{
				_force = value;
			}
		}
	};

}
