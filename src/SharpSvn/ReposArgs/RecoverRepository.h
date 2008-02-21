// $Id: Add.h 272 2008-02-14 17:46:34Z bhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnRepositoryClient" />'s RecoverRepository method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnRecoverRepositoryArgs : public SvnClientArgs
	{
		bool _nonBlocking;
	public:
		SvnRecoverRepositoryArgs()
		{
		}

		property bool NonBlocking
		{
			bool get()
			{
				return _nonBlocking;
			}
			void set(bool value)
			{
				_nonBlocking = value;
			}
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Unknown;
			}
		}
	};
}