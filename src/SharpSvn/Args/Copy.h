// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

	/// <summary>Extended Parameter container of SvnClient.Copy(SvnTarget^,String^,SvnCopyArgs^)" /> and
	/// <see cref="SvnClient::RemoteCopy(SvnTarget^,Uri^,SvnCopyArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnCopyArgs : public SvnClientArgsWithCommit
	{
		bool _makeParents;
		bool _alwaysCopyBelow;

	public:
		SvnCopyArgs()
		{
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Copy;
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

		/// <summary>Always copies the result to below the target (this behaviour is always used if multiple targets are provided)</summary>
		property bool AlwaysCopyAsChild
		{
			bool get()
			{
				return _alwaysCopyBelow;
			}
			void set(bool value)
			{
				_alwaysCopyBelow = value;
			}
		}
	};

}
