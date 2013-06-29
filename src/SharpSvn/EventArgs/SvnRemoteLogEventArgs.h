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

namespace SharpSvn {
	namespace Remote {

	public ref class SvnRemoteLogEventArgs : public SvnLoggingEventArgs
	{
		initonly bool _hasChildren;
		initonly int _mergeLevel;
	internal:
		SvnRemoteLogEventArgs(svn_log_entry_t *entry, int mergeLevel, AprPool ^pool)
			: SvnLoggingEventArgs(entry, pool)
		{
			_hasChildren = entry->has_children ? true : false;
			_mergeLevel = mergeLevel;
		}

	public:
		/// <summary>Set to true when the following items are merged-child items of this item.</summary>
		property bool HasChildren
		{
			bool get()
			{
				return _hasChildren;
			}
		}

		/// <summary>Gets the nesting level of the logs via merges</summary>
		property int MergeLogNestingLevel
		{
			int get()
			{
				return _mergeLevel;
			}
		}

	};
}
}