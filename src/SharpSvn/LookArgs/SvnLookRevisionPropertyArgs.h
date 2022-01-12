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

#include "SvnLookClientArgs.h"

namespace SharpSvn {

    /// <summary>Extended Parameter container of <see cref="SvnLookClient" />'s Property method</summary>
    /// <threadsafety static="true" instance="false"/>
    public ref class SvnLookRevisionPropertyArgs : public SvnLookClientArgs
    {
        bool _noRefresh;
    public:
        SvnLookRevisionPropertyArgs()
        {
        }

        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::Unknown;
            }
        }

        property bool NoRefresh
        {
            bool get() { return _noRefresh; }
            void set(bool value)
            {
                _noRefresh = value;
            }
        }
    };
}
