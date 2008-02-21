// $Id: SvnClientArgs.h 290 2008-02-19 11:13:45Z bhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {

	public ref class SvnMergeDiffArgs : SvnClientArgs
	{
		bool _dontConsiderInheritance;

	public:
		SvnMergeDiffArgs ()
		{}

		property bool ConsiderInheritance
		{
			bool get()
			{
				return !_dontConsiderInheritance;
			}
			void set(bool value)
			{
				_dontConsiderInheritance = !value;
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

	public ref class SvnMergeRemoveArgs : SvnClientArgs
	{
		bool _dontConsiderInheritance;

	public:
		SvnMergeRemoveArgs ()
		{}

		property bool ConsiderInheritance
		{
			bool get()
			{
				return !_dontConsiderInheritance;
			}
			void set(bool value)
			{
				_dontConsiderInheritance = !value;
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