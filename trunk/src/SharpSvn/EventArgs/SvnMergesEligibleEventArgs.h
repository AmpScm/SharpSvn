// $Id$
//
// Copyright 2007-2008 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

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