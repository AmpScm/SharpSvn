// $Id: SvnMergesMergedEventArgs.h 944 2008-12-12 10:20:50Z rhuijben $
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
#include "EventArgs/SvnListEventArgs.h"

namespace SharpSvn {

	public ref class SvnRemoteStatEventArgs : public SvnEventArgs
	{
		SvnDirEntry^ _entry;
	internal:
		SvnRemoteStatEventArgs(svn_dirent_t *dirent)
		{
			UNUSED_ALWAYS(dirent);
			_entry = gcnew SvnDirEntry(dirent);
		}

	public:
		property SvnDirEntry^ Entry
		{
			SvnDirEntry^ get()
			{
				return _entry;
			}
		}
	};
}