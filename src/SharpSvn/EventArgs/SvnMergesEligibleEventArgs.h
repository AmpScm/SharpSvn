// $Id: SvnListEventArgs.h 509 2008-04-04 20:54:32Z rhuijben $
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnLogEventArgs.h"
#include "SvnMergeInfo.h"

namespace SharpSvn {

	public ref class SvnMergesEligibleEventArgs : public SvnLoggingEventArgs
	{
		initonly SvnUriTarget^ _source;
	internal:
		SvnMergesEligibleEventArgs(svn_log_entry_t *entry, SvnUriTarget^ source, AprPool ^pool)
			: SvnLoggingEventArgs(entry, pool)
		{
			if(!source)
				throw gcnew ArgumentNullException("source");

			_source = source;
		}

	public:
		property Uri^ SourceUri
		{
			Uri^ get()
			{
				return _source->Uri;
			}
		}

		property SvnUriTarget^ SourceTarget
		{
			SvnUriTarget^ get()
			{
				return _source;
			}
		}

		SvnRevisionRange^ AsRange()
		{
			return gcnew SvnRevisionRange(Revision-1, Revision);
		}
	};
}