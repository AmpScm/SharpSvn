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

    using namespace System;

    /// <summary>Extended Parameter container of <see cref="SvnClient::SetRevisionProperty(SvnUriTarget^,String^,SvnSetRevisionPropertyArgs^,String^)" /></summary>
    /// <threadsafety static="true" instance="false"/>
    public ref class SvnSetRevisionPropertyArgs : public SvnClientArgs
    {
        bool _force;
        String ^_oldValue;
        array<Byte> ^_oldValueBytes;
    public:
        SvnSetRevisionPropertyArgs()
        {
        }

        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::SetRevisionProperty;
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

        property String ^OldValue
        {
            String ^get()
            {
                return _oldValue;
            }
            void set(String ^value)
            {
                _oldValue = value;
                if (value)
                    _oldValueBytes = nullptr;
            }
        }

        property array<Byte> ^OldRawValue
        {
            array<Byte> ^get()
            {
                return _oldValueBytes;
            }
            void set(array<Byte> ^ value)
            {
                _oldValueBytes = (array<Byte> ^)(value ? value->Clone() : nullptr);
                if (value)
                    _oldValue = nullptr;
            }
        }
    };

}
