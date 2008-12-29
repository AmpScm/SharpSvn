// $Id: SvnConflictEventArgs.h 944 2008-12-12 10:20:50Z rhuijben $
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

// Included from SvnClientEventArgs.h

namespace SharpSvn {
	ref class Delta::SvnDeltaEditor;

	public ref class SvnReplayRevisionStartEventArgs : public SvnCancelEventArgs
	{
		initonly __int64 _revision;
		Delta::SvnDeltaEditor^ _editor;
	internal:
		SvnReplayRevisionStartEventArgs(__int64 revision, Delta::SvnDeltaEditor^ editor, apr_hash_t* revprops)
		{
			UNUSED_ALWAYS(revprops);
			_revision = revision;
			_editor = editor;
		}

	public:
		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}
		property Delta::SvnDeltaEditor^ Editor
		{
			Delta::SvnDeltaEditor^ get()
			{
				return _editor;
			}
			void set(Delta::SvnDeltaEditor^ value)
			{
				_editor = value;
			}
		}

		// TODO: Add revision property support
	};

	public ref class SvnReplayRevisionEndEventArgs : public SvnCancelEventArgs
	{
		initonly __int64 _revision;
	internal:
		SvnReplayRevisionEndEventArgs(__int64 revision, apr_hash_t *revprops)
		{
			UNUSED_ALWAYS(revprops);
			_revision = revision;
		}

	public:
		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}
	};

}