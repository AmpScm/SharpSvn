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

    /// <summary>Extended Parameter container of <see cref="SvnRepositoryClient" />'s DeleteRepository method</summary>
    /// <threadsafety static="true" instance="false"/>
    public ref class SvnVerifyRepositoryArgs : public SvnRepositoryClientArgs
    {
        __int64 _startRev;
        __int64 _endRev;

    public:
        SvnVerifyRepositoryArgs()
        {
            _startRev = SVN_INVALID_REVNUM;
            _endRev = SVN_INVALID_REVNUM;
        }

        property __int64 StartRevision
        {
            __int64 get()
            {
                return _startRev;
            }
            void set(__int64 value)
            {
                _startRev = value;
            }
        }

        property __int64 EndRevision
        {
            __int64 get()
            {
                return _endRev;
            }
            void set(__int64 value)
            {
                _endRev = value;
            }
        }


        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::Unknown;
            }
        }
    };
}}