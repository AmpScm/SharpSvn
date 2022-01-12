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

#include "svn_repos.h"


namespace SharpSvn {

    public enum class SvnRepositoryIdType
    {
        Default = svn_repos_load_uuid_default,
        Ignore = svn_repos_load_uuid_ignore,
        Force = svn_repos_load_uuid_force
    };

    /// <summary>Extended Parameter container of <see cref="SvnRepositoryClient" />'s CreateRepository method</summary>
    /// <threadsafety static="true" instance="false"/>
    public ref class SvnLoadRepositoryArgs : public SvnRepositoryClientArgs
    {
        SvnRepositoryIdType _loadIdType;
        String^ _parentDir;
        bool _noPreCommitHook;
        bool _noPostCommitHook;
        bool _noPropertyValidation;
        bool _noDateVerify;
        bool _normalizeProperties;
        __int64 _startRev;
        __int64 _endRev;

    public:
        SvnLoadRepositoryArgs()
        {
            _startRev = SVN_INVALID_REVNUM;
            _endRev = SVN_INVALID_REVNUM;
        }

        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::Unknown;
            }
        }

        property SvnRepositoryIdType LoadIdType
        {
            SvnRepositoryIdType get()
            {
                return _loadIdType;
            }
            void set(SvnRepositoryIdType value)
            {
                _loadIdType = EnumVerifier::Verify(value);
            }
        }

        property bool RunPreCommitHook
        {
            bool get()
            {
                return !_noPreCommitHook;
            }
            void set(bool value)
            {
                _noPreCommitHook = !value;
            }
        }

        property bool RunPostCommitHook
        {
            bool get()
            {
                return !_noPostCommitHook;
            }
            void set(bool value)
            {
                _noPostCommitHook = !value;
            }
        }

        property bool VerifyPropertyValues
        {
            bool get()
            {
                return !_noPropertyValidation;
            }
            void set(bool value)
            {
                _noPropertyValidation = !value;
            }
        }

        property bool VerifyDates
        {
            bool get()
            {
                return !_noDateVerify;
            }
            void set(bool value)
            {
                _noDateVerify = !value;
            }
        }

        property String^ ImportParent
        {
            String^ get()
            {
                return _parentDir;
            }
            void set(String^ value)
            {
                _parentDir = value;
            }
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

        property bool NormalizeProperties
        {
            bool get()
            {
                return _normalizeProperties;
            }
            void set(bool value)
            {
                _normalizeProperties = value;
            }
        }

    };
}
