// $Id$
// Copyright (c) SharpSvn Project 2007-2008
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnLogEventArgs.h"
#include "SvnMergeInfo.h"

namespace SharpSvn {

	public ref class SvnMergesEligibleEventArgs : public SvnLoggingEventArgs
	{
		initonly SvnTarget^ _source;
	internal:
		SvnMergesEligibleEventArgs(svn_log_entry_t *entry, SvnTarget^ source, AprPool ^pool)
			: SvnLoggingEventArgs(entry, pool)
		{
			if (!source)
				throw gcnew ArgumentNullException("source");

			_source = source;
		}

	public:
		/// <summary>Gets the specified SourceUri or <c>null</c> if the source was a local path</summary>
		property Uri^ SourceUri
		{
			Uri^ get()
			{
				SvnUriTarget^ trg = dynamic_cast<SvnUriTarget^>(_source);
				if (trg)
					return trg->Uri;
				else
					return nullptr;
			}
		}

		/// <summary>Gets the merge source passed to the SvnClient call</summary>
		property SvnTarget^ SourceTarget
		{
			SvnTarget^ get()
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