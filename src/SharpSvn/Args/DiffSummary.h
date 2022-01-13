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

    /// <summary>Extended Parameter container for SvnClient.DiffSummary</summary>
    /// <threadsafety static="true" instance="false"/>
    public ref class SvnDiffSummaryArgs : public SvnClientArgs
    {
        bool _noticeAncestry;
        SvnDepth _depth;
        SvnChangeListCollection^ _changelists;
    public:
        SvnDiffSummaryArgs()
        {
            _depth = SvnDepth::Infinity;
        }

        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::DiffSummary;
            }
        }

    public:
        DECLARE_EVENT(SvnDiffSummaryEventArgs^, DiffSummary);

    protected:
        virtual void OnDiffSummary(SvnDiffSummaryEventArgs^ e)
        {
            DiffSummary(this, e);
        }

    internal:
        void RaiseDiffSummary(SvnDiffSummaryEventArgs^ e)
        {
            OnDiffSummary(e);
        }

    public:
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

        property bool IgnoreAncestry
        {
            bool get()
            {
                return !_noticeAncestry;
            }
            void set(bool value)
            {
                _noticeAncestry = !value;
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

    internal:
        const char* _fromUri;
        const char* _toUri;
    };


}
