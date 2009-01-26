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