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

    /// <summary>Extended Parameter container for SvnClient.Blame</summary>
    /// <threadsafety static="true" instance="false"/>
    public ref class SvnGetCapabilitiesArgs : public SvnClientArgs
    {
        initonly Collection<SvnCapability>^ _capabilities;
        bool _retrieveAll;

    public:
        SvnGetCapabilitiesArgs()
        {
            _capabilities = gcnew Collection<SvnCapability>();
        }

        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::Unknown;
            }
        }

    public:
        property bool RetrieveAllCapabilities
        {
            bool get()
            {
                return _retrieveAll;
            }
            void set(bool value)
            {
                _retrieveAll = value;
            }
        }

        property Collection<SvnCapability>^ Capabilities
        {
            Collection<SvnCapability>^ get()
            {
                return _capabilities;
            }
        }
    };

}
