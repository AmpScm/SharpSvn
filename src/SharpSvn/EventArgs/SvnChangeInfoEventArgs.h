// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {

	public ref class SvnChangeInfoEventArgs : public SvnLoggingEventArgs
	{
		initonly __int64 _baseRevision;
	internal:
		SvnChangeInfoEventArgs(svn_log_entry_t *entry, __int64 baseRevision, AprPool ^pool)
			: SvnLoggingEventArgs(entry, pool)
		{
			_baseRevision = baseRevision;
		}

	public:
		/// <summary>Gets the revision the change is based on</summary>
		property __int64 BaseRevision
		{
			__int64 get()
			{
				return _baseRevision;
			}
		}
	};
}