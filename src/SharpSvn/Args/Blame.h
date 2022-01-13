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

    /// <summary>Extended Parameter container for SvnClient.Blame</summary>
    /// <threadsafety static="true" instance="false"/>
    public ref class SvnBlameArgs : public SvnClientArgs
    {
        SvnRevision ^_start;
        SvnRevision ^_end;
        SvnIgnoreSpacing _ignoreSpace;
        bool _ignoreEol;
        bool _ignoreMime;
        bool _includeMergedRevisions;

    public:
        SvnBlameArgs()
        {
            _start = SvnRevision::None;
            _end = SvnRevision::None;
        }

        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::Blame;
            }
        }

    public:
        DECLARE_EVENT(SvnBlameEventArgs^, Blame);

    protected:
        virtual void OnBlame(SvnBlameEventArgs^ e)
        {
            Blame(this, e);
        }

    internal:
        void RaiseBlame(SvnBlameEventArgs^ e)
        {
            OnBlame(e);
        }

    public:
        /// <summary>Gets or sets the blame range as <see cref="SvnRevisionRange" /></summary>
        property SvnRevisionRange^ Range
        {
            SvnRevisionRange^ get()
            {
                return gcnew SvnRevisionRange(Start, End);
            }

            void set(SvnRevisionRange^ value)
            {
                if (!value)
                {
                    Start = nullptr;
                    End = nullptr;
                }
                else
                {
                    Start = value->StartRevision;
                    End = value->EndRevision;
                }
            }
        }

        property SvnRevision^ Start
        {
            SvnRevision^ get()
            {
                return _start;
            }
            void set(SvnRevision^ value)
            {
                if (value)
                    _start = value;
                else
                    _start = SvnRevision::None;
            }
        }

        property SvnRevision^ End
        {
            SvnRevision^ get()
            {
                return _end;
            }
            void set(SvnRevision^ value)
            {
                if (value)
                    _end = value;
                else
                    _end = SvnRevision::None;
            }
        }

        property SvnIgnoreSpacing IgnoreSpacing
        {
            SvnIgnoreSpacing get()
            {
                return _ignoreSpace;
            }
            void set(SvnIgnoreSpacing value)
            {
                _ignoreSpace = value;
            }
        }

        property bool IgnoreLineEndings
        {
            bool get()
            {
                return _ignoreEol;
            }
            void set(bool value)
            {
                _ignoreEol = value;
            }
        }

        property bool IgnoreMimeType
        {
            bool get()
            {
                return _ignoreMime;
            }
            void set(bool value)
            {
                _ignoreMime = value;
            }
        }

        property bool RetrieveMergedRevisions
        {
            bool get()
            {
                return _includeMergedRevisions;
            }
            void set(bool value)
            {
                _includeMergedRevisions = value;
            }
        }
    };


}
