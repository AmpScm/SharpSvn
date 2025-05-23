// Copyright 2007-2025 The SharpSvn Project
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

    public ref class SvnProgressEventArgs sealed : public SvnEventArgs
    {
        initonly __int64 _progress;
        initonly __int64 _totalProgress;

    public:
        SvnProgressEventArgs(__int64 progress, __int64 totalProgress)
        {
            _progress = progress;
            _totalProgress = totalProgress;
        }

    public:
        property __int64 Progress
        {
            __int64 get()
            {
                return _progress;
            }
        }

        property __int64 TotalProgress
        {
            __int64 get()
            {
                return _totalProgress;
            }
        }

        /// <summary>Serves as a hashcode for the specified type</summary>
        virtual int GetHashCode() override
        {
            return _progress.GetHashCode();
        }
    };

}
