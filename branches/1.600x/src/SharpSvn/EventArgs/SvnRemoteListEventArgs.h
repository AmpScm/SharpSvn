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
	namespace Remote {
	ref class SvnRemoteSession;

	public ref class SvnRemoteListEventArgs : public SvnEventArgs, public ISvnRepositoryListItem
	{
		initonly String^ _name;
		initonly SvnDirEntry^ _entry;
		initonly __int64 _revision;
		initonly String^ _relPath;
		initonly System::Uri^ _sessionUri;
		System::Uri^ _uri;
		
	internal:
		SvnRemoteListEventArgs(String^ name, const svn_dirent_t *dirent, svn_revnum_t revno, Uri^ sessionUri, String^ relPath)
		{
			_name = name;
			_entry = gcnew SvnDirEntry(dirent);
			_revision = revno;
			_relPath = relPath;
			_sessionUri = sessionUri;
		}

	public:
		property String^ Name
		{
			String^ get()
			{
				return _name;
			}
		}

		property __int64 RetrievedRevision
		{
			__int64 get()
			{
				return _revision;
			}
		}

		property String^ Path
		{
			String^ get()
			{
				return _relPath + Name;
			}
		}

		property System::Uri^ Uri
		{
			virtual System::Uri^ get() sealed
			{
				if (!_uri && Entry)
				{
					if (Path->Length == 0)
						_uri = _sessionUri;
					else if (Entry->NodeKind == SvnNodeKind::Directory)
						_uri = gcnew System::Uri(_sessionUri, SvnBase::PathToUri(Path + "/"));
					else
						_uri = gcnew System::Uri(_sessionUri, SvnBase::PathToUri(Path));
				}

				return _uri;
			}
		}

		property System::Uri^ BaseUri
		{
			System::Uri^ get()
			{
				return _sessionUri;
			}
		}

		property SvnDirEntry^ Entry
		{
			virtual SvnDirEntry^ get() sealed
			{
				return _entry;
			}
		}

	protected public:
		/// <summary>Detaches the SvnEventArgs from the unmanaged storage; optionally keeping the property values for later use</summary>
		/// <description>After this method is called all properties are either stored managed, or are no longer readable</description>
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if (keepProperties)
				{
                    GC::KeepAlive(Entry);
				}
			}
			finally
			{
                if (_entry != nullptr)
				    _entry->Detach(keepProperties);

				__super::Detach(keepProperties);
			}
		}
	};
}
}
