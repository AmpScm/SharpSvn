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

    /// <summary>Extended Parameter container of <see cref="SvnClient::Write(SvnTarget^, Stream^, SvnWriteArgs^)" /></summary>
    /// <threadsafety static="true" instance="false"/>
    public ref class SvnWriteArgs : public SvnClientArgs
    {
        SvnRevision^ _revision;
    bool _ignoreKeywords;
    public:
        SvnWriteArgs()
        {
            _revision = SvnRevision::None;
        }

        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::Write;
            }
        }

        property SvnRevision^ Revision
        {
            SvnRevision^ get()
            {
                return _revision;
            }
            void set(SvnRevision^ value)
            {
                if (value)
                    _revision = value;
                else
                    _revision = SvnRevision::None;
            }
        }

    property bool IgnoreKeywords
        {
            bool get()
            {
                return _ignoreKeywords;
            }
            void set(bool value)
            {
                _ignoreKeywords = value;
            }
        }
    };

}
