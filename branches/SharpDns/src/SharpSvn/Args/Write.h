// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

/// <summary>Extended Parameter container of <see cref="SvnClient::Write(SvnTarget^, Stream^, SvnWriteArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnWriteArgs : public SvnClientArgs
	{
		SvnRevision^ _revision;
	public:
		SvnWriteArgs()
		{
			_revision = SvnRevision::None;
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Write;
			}
		}

		property SvnRevision^ Revision
		{
			SvnRevision^ get()
			{
				return _revision;
			}
			void set(SvnRevision^ value)
			{
				if (value)
					_revision = value;
				else
					_revision = SvnRevision::None;
			}
		}
	};

}
