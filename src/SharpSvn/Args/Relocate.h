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

    /// <summary>Extended Parameter container of <see cref="SvnClient::Relocate(String^, Uri^, Uri^, SvnRelocateArgs^)" /></summary>
    /// <threadsafety static="true" instance="false"/>
    public ref class SvnRelocateArgs : public SvnClientArgs
    {
        bool _ignoreExternals;
    public:
        SvnRelocateArgs()
        {
        }

        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::Relocate;
            }
        }

        property bool IgnoreExternals
        {
            bool get()
            {
                return _ignoreExternals;
            }
            void set(bool value)
            {
                _ignoreExternals = value;
            }
        }
    };
}
