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
#include "SvnMergeInfo.h"

namespace SharpSvn {
	namespace Remote {
	ref class SvnRemoteSession;

	public ref class SvnRemoteLocationSegmentEventArgs : public SvnCancelEventArgs
	{
        initonly __int64 _start;
        initonly __int64 _end;
        initonly Uri^ _reposUri;
        initonly int _nOffset;
        const char *_reposPath;
        String^ _path;
        String^ _reposRelPath;
        Uri^ _uri;
        SvnRevisionRange^ _range;

	internal:
        SvnRemoteLocationSegmentEventArgs(const svn_location_segment_t *segment, Uri^ reposUri, int nOffset)
		{
            _reposPath = segment->path;
            _start = segment->range_start;
            _end = segment->range_end;
            _reposUri = reposUri;
            _nOffset = nOffset;
		}

    public:

        property String^ RepositoryPath
        {
            String^ get()
            {
                if (!_reposRelPath && _reposPath)
                    _reposRelPath = SvnBase::Utf8_PtrToString(_reposPath);

                return _reposRelPath;
            }
        }

        property System::Uri^ Uri
        {
            System::Uri^ get()
            {
                if (!_uri && _reposUri && RepositoryPath)
                    _uri = SvnTools::AppendPathSuffix(_reposUri, RepositoryPath);

                return _uri;
            }
        }

        property __int64 StartRevision
        {
            __int64 get()
            {
                return _start;
            }
        }

        property __int64 EndRevision
        {
            __int64 get()
            {
                return _end;
            }
        }

        /// <summary>Retrieve StartRevision-EndRevision as a <see cref="SvnRevisionRange" /></summary>
        property SvnRevisionRange^ Range
        {
            SvnRevisionRange^ get()
            {
                if (!_range)
                    _range = gcnew SvnRevisionRange(StartRevision, EndRevision);
                return _range;
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
                    GC::KeepAlive(RepositoryPath);
				}
			}
			finally
			{
                _reposPath = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};
}
}
