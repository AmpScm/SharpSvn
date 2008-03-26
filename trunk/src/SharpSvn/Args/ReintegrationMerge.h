// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnClient::ReintegrationMerge(String^,SvnTarget^, SvnReintegrationMergeArgs^)" /></summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnReintegrationMergeArgs : public SvnMergeBaseArgs
	{
		bool _dryRun;
	public:
		SvnReintegrationMergeArgs()
		{
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::ReintegrationMerge;
			}
		}

		property bool DryRun
		{
			bool get()
			{
				return _dryRun;
			}
			void set(bool value)
			{
				_dryRun = value;
			}
		}
	};

}
