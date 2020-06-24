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

    public ref class SvnMergeDiffArgs : SvnClientArgs
    {
        bool _dontConsiderInheritance;

    public:
        SvnMergeDiffArgs ()
        {}

        property bool ConsiderInheritance
        {
            bool get()
            {
                return !_dontConsiderInheritance;
            }
            void set(bool value)
            {
                _dontConsiderInheritance = !value;
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

    public ref class SvnMergeRemoveArgs : SvnClientArgs
    {
        bool _dontConsiderInheritance;

    public:
        SvnMergeRemoveArgs ()
        {}

        property bool ConsiderInheritance
        {
            bool get()
            {
                return !_dontConsiderInheritance;
            }
            void set(bool value)
            {
                _dontConsiderInheritance = !value;
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

    public ref class SvnMergeIntersectArgs : SvnClientArgs
    {
        bool _dontConsiderInheritance;

    public:
        SvnMergeIntersectArgs ()
        {}

        property bool ConsiderInheritance
        {
            bool get()
            {
                return !_dontConsiderInheritance;
            }
            void set(bool value)
            {
                _dontConsiderInheritance = !value;
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

}
