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

	public ref class SvnRemoteListLockEventArgs : public SvnEventArgs
	{
		initonly String^ _reposRelpath;
        initonly int _nOffset;
        initonly Uri^ _sessionUri;
        SvnLockInfo^ _lock;
        const svn_lock_t *_lockInfo;
        String^ _path;
        Uri^ _uri;
		
	internal:
		SvnRemoteListLockEventArgs(String^ reposRelpath, const svn_lock_t *lock_info, Uri^ sessionUri, int nOffset)
		{
			_reposRelpath = reposRelpath;
            _sessionUri = sessionUri;
            _nOffset = nOffset;

            _lockInfo = lock_info;
		}

	public:
		property String^ Path
		{
			String^ get()
			{
                if (!_path)
                    _path = _reposRelpath->Substring(_nOffset);

				return _path;
			}
		}

        property String^ RepositoryPath
        {
            String^ get()
            {
                return _reposRelpath;
            }
        }

        property System::Uri^ Uri
        {
            System::Uri^ get()
            {
                if (!_uri && _sessionUri && Path)
                    _uri = SvnTools::AppendPathSuffix(_sessionUri, Path);

                return _uri;
            }
        }

		property SvnLockInfo^ Lock
		{
			SvnLockInfo^ get()
			{
				if (!_lock && _lockInfo)
                    _lock = gcnew SvnLockInfo(_lockInfo, false);

                return _lock;
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
                    GC::KeepAlive(Lock);
				}
			}
			finally
			{
                if (_lock != nullptr)
                    _lock->Detach(keepProperties);

				__super::Detach(keepProperties);
			}
		}
	};
}
}
