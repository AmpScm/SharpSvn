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

namespace SharpSvn {

    /// <summary>Extended Parameter container of <see cref="SvnClient::Merge(String^,SvnTarget^, SvnRevisionRange^, SvnMergeArgs^)" /></summary>
    /// <threadsafety static="true" instance="false"/>
    public ref class SvnMergeArgs : public SvnMergeBaseArgs
    {
        SvnDepth _depth;
        bool _ignoreAncestry;
        bool _force;
        bool _recordOnly;
        bool _dryRun;
        bool _checkForMixedRevisions;
        Nullable<bool> _ignoreMergeInfo;

    public:
        SvnMergeArgs()
        {
            _depth = SvnDepth::Unknown;
        }

        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::Merge;
            }
        }

        property SvnDepth Depth
        {
            SvnDepth get()
            {
                return _depth;
            }
            void set(SvnDepth value)
            {
                _depth = EnumVerifier::Verify(value);
            }
        }

        property Nullable<bool> IgnoreMergeInfo
        {
            Nullable<bool> get()
            {
                return _ignoreMergeInfo;
            }
            void set(Nullable<bool> value)
            {
                _ignoreMergeInfo = value;
            }
        }

        property bool IgnoreAncestry
        {
            bool get()
            {
                return _ignoreAncestry;
            }
            void set(bool value)
            {
                _ignoreAncestry = value;
            }
        }

        property bool Force
        {
            bool get()
            {
                return _force;
            }
            void set(bool value)
            {
                _force = value;
            }
        }

        property bool RecordOnly
        {
            bool get()
            {
                return _recordOnly;
            }
            void set(bool value)
            {
                _recordOnly = value;
            }
        }

        property bool DryRun
        {
            bool get()
            {
                return _dryRun;
            }
            void set(bool value)
            {
                _dryRun = value;
            }
        }

        property bool CheckForMixedRevisions
        {
            bool get()
            {
                return _checkForMixedRevisions;
            }
            void set(bool value)
            {
                _checkForMixedRevisions = value;
            }
        }
    };

}
