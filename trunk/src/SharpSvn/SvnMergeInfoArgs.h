// $Id$
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

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Unknown;
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

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Unknown;
			}
		}
	};

}