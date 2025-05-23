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

    /// <summary>Extended Parameter container for SvnClient.ListChangeList</summary>
    /// <threadsafety static="true" instance="false"/>
    public ref class SvnListChangeListArgs : public SvnClientArgs
    {
        SvnDepth _depth;
        SvnChangeListCollection^ _changelists;
    public:
        SvnListChangeListArgs()
        {
            _depth = SvnDepth::Infinity;
        }

        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::ListChangeList;
            }
        }

    public:
        event EventHandler<SvnListChangeListEventArgs^>^ ListChangeList;

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

        /// <summary>Gets the list of changelist-names</summary>
        property SvnChangeListCollection^ ChangeLists
        {
            SvnChangeListCollection^ get()
            {
                if (!_changelists)
                    _changelists = gcnew SvnChangeListCollection();
                return _changelists;
            }
        }

    protected public:
        virtual void OnListChangeList(SvnListChangeListEventArgs^ e)
        {
            ListChangeList(this, e);
        }
    };

}
