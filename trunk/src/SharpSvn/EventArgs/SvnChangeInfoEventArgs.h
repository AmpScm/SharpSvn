// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {

	public ref class SvnChangeInfoEventArgs : public SvnLoggingEventArgs
	{
	internal:
		SvnChangeInfoEventArgs(svn_log_entry_t *entry, AprPool ^pool)
			: SvnLoggingEventArgs(entry, pool)
		{
		}
	};
}