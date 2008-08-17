// $Id: SvnMergesEligibleEventArgs.h 650 2008-08-07 11:28:07Z rhuijben $
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnLookClientArgs.h"

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnLookClient" />'s ChangeInfo command</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnChangeInfoArgs : public SvnLookClientArgs
	{
		bool _noLogChangedPaths;
	public:
		SvnChangeInfoArgs()
		{
		}

		event EventHandler<SvnChangeInfoEventArgs^>^ ChangeInfo;
	protected public:
		void OnChangeInfo(SvnChangeInfoEventArgs^ e)
		{
			ChangeInfo(this, e);
		}
	public:

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Unknown;
			}
		}

	public:
		/// <summary>Gets or sets a boolean indicating whether the paths changed in the revision should be provided</summary>
		/// <remarks>Defaults to true</remarks>
		property bool RetrieveChangedPaths
		{
			bool get()
			{
				return !_noLogChangedPaths;
			}
			void set(bool value)
			{
				_noLogChangedPaths = !value;
			}
		}
	};
}