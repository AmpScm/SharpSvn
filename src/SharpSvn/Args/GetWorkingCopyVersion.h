// $Id: GetWorkingCopyState.h 500 2008-03-26 17:47:22Z rhuijben $
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once



namespace SharpSvn {

	public ref class SvnGetWorkingCopyVersionArgs : public SvnClientArgs
	{
		String^ _trailUrl;
		bool _committedRevisions;

	public:
		/// <summary>Gets the <see cref="SvnCommandType" /> of the command</summary>
		virtual property SvnCommandType CommandType
		{
			SvnCommandType get() override sealed
			{
				return SvnCommandType::GetWorkingCopyVersion;
			}
		}

	public:
		property String^ Trail
		{
			String^ get()
			{
				return _trailUrl;
			}
			void set(String^ value)
			{
				_trailUrl = value;
			}
		}

		property bool UseCommittedRevisions
		{
			bool get()
			{
				return _committedRevisions;
			}
			void set(bool value)
			{
				_committedRevisions = value;
			}
		}
	};
}