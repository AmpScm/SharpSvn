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

    public ref class SvnRevisionPropertyNameCollection sealed : KeyedCollection<String^, String^>
    {
    internal:
        SvnRevisionPropertyNameCollection(bool initialEmpty);

    public:
        void AddDefaultProperties();

    protected:
        virtual String^ GetKeyForItem(String^ item) override
        {
            return item;
        }

    public:
        static initonly String^ Author = SVN_PROP_REVISION_AUTHOR;
        static initonly String^ Date = SVN_PROP_REVISION_DATE;
        static initonly String^ Log = SVN_PROP_REVISION_LOG;
    };


    /// <summary>Extended Parameter container of SvnClient.Log</summary>
    /// <threadsafety static="true" instance="false"/>
    public ref class SvnLogArgs : public SvnClientArgs
    {
        SvnRevision^ _pegRevision;
        SvnRevision^ _start;
        SvnRevision^ _end;
        int _limit;
        bool _noLogChangedPaths;
        bool _strictNodeHistory;
        bool _includeMerged;
        bool _retrieveAllProperties;
        Uri^ _baseUri;
        SvnRevisionPropertyNameCollection^ _retrieveProperties;
        Collection<SvnRevisionRange^>^ _ranges;

    internal:
        int _mergeLogLevel; // Used by log handler to provide mergeLogLevel
        Uri^ _searchRoot;

    public:
        DECLARE_EVENT(SvnLogEventArgs^, Log);

    protected public:
        virtual void OnLog(SvnLogEventArgs^ e)
        {
            Log(this, e);
        }

    public:
        /// <summary>Initializes a new <see cref="SvnLogArgs" /> instance with default properties</summary>
        SvnLogArgs()
        {
            _start = SvnRevision::None;
            _end = SvnRevision::None;
            _pegRevision = SvnRevision::None;
            //_limit = 0;
            //_noLogChangedPaths = false;
            //_strictHistory = false;
        }

        /// <summary>Initializes a new <see cref="SvnLogArgs" /> instance with the specified range</summary>
        SvnLogArgs(SvnRevisionRange^ range)
        {
            if (!range)
                throw gcnew ArgumentNullException("range");

            Range = range;
            _pegRevision = SvnRevision::None;
        }

        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::Log;
            }
        }

        /// <summary>Gets the revision in which the Url's are evaluated (Aka peg revision)</summary>
        property SvnRevision^ OperationalRevision
        {
            SvnRevision^ get()
            {
                return _pegRevision;
            }
            void set(SvnRevision^ value)
            {
                if (value)
                    _pegRevision = value;
                else
                    _pegRevision = SvnRevision::None;
            }
        }

        /// <summary>Gets or sets the range specified by <see cref="Start" />-<see cref="End" /> wrapped as a <see cref="SvnRevisionRange" /></summary>
        /// <remarks>New code should either use <see cref="Start" /> and <see cref="End" /> or <see cref="Ranges" /></remarks>
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
                if (_ranges)
                    return (_ranges->Count == 1) ? _ranges[0]->StartRevision : SvnRevision::None;
                else
                    return _start ? _start : SvnRevision::None;
            }

            void set(SvnRevision^ value)
            {
                _start = value;

                if (_ranges)
                {
                    _end = End;
                    _ranges->Clear();
                    _ranges->Add(gcnew SvnRevisionRange(Start, End));
                }
            }
        }

        property SvnRevision^ End
        {
            SvnRevision^ get()
            {
                if (_ranges)
                    return (_ranges->Count == 1) ? _ranges[0]->EndRevision : SvnRevision::None;
                else
                    return _end ? _end : SvnRevision::None;
            }

            void set(SvnRevision^ value)
            {
                _end = value;

                if (_ranges)
                {
                    _start = Start;
                    _ranges->Clear();
                    _ranges->Add(gcnew SvnRevisionRange(Start, End));
                }
            }
        }

        property Collection<SvnRevisionRange^>^ Ranges
        {
            Collection<SvnRevisionRange^>^ get()
            {
                if (!_ranges)
                {
                    SvnRevision^ start = Start;
                    SvnRevision^ end = End;

                    _ranges = gcnew Collection<SvnRevisionRange^>();

                    if (start != SvnRevision::None || end != SvnRevision::None)
                        _ranges->Add(gcnew SvnRevisionRange(start, end));
                }

                return _ranges;
            }
        }

        /// <summary>Gets or sets the limit on the number of log items fetched</summary>
        property int Limit
        {
            int get()
            {
                return _limit;
            }

            void set(int value)
            {
                _limit = (value >= 0) ? value : 0;
            }
        }

        /// <summary>Gets or sets a boolean indicating whether the paths changed in the revision should be provided</summary>
        property bool RetrieveChangedPaths
        {
            bool get()
            {
                return !_noLogChangedPaths;
            }
            void set(bool value)
            {
                _noLogChangedPaths = !value;
            }
        }

        /// <summary>Gets or sets a boolean indicating whether only the history of this exact node should be fetched (Aka stop on copying)</summary>
        /// <remarks>If @a StrictNodeHistory is set, copy history (if any exists) will
        /// not be traversed while harvesting revision logs for each target. </remarks>
        property bool StrictNodeHistory
        {
            bool get()
            {
                return _strictNodeHistory;
            }
            void set(bool value)
            {
                _strictNodeHistory = value;
            }
        }

        /// <summary>Gets or sets a boolean indicating whether the merged revisions should be fetched instead of the node changes</summary>
        property bool RetrieveMergedRevisions
        {
            bool get()
            {
                return _includeMerged;
            }
            void set(bool value)
            {
                _includeMerged = value;
            }
        }

        /// <summary>Gets or sets the base uri to which relative Uri's are relative</summary>
        property Uri^ BaseUri
        {
            Uri^ get()
            {
                return _baseUri;
            }
            void set(Uri^ value)
            {
                _baseUri = value;
            }
        }

        /// <summary>Gets the list of properties to retrieve. Only SVN 1.5+ repositories allow adding custom properties to this list</summary>
        /// <remarks>This list defaults to the author, date, logmessage properties. Clear the list to retrieve no properties,
        /// or set RetrieveAllProperties to true to retrieve all properties.
        /// </remarks>
        property SvnRevisionPropertyNameCollection^ RetrieveProperties
        {
            SvnRevisionPropertyNameCollection^ get();
        }

        /// <summary>Gets or sets a boolean indication whether to retrieve all revision properties</summary>
        /// <remarks>Default value: false</remarks>
        property bool RetrieveAllProperties
        {
            bool get()
            {
                return _retrieveAllProperties;
            }
            void set(bool value)
            {
                _retrieveAllProperties = value;
            }
        }

    internal:
        property bool RetrievePropertiesUsed
        {
            bool get()
            {
                return _retrieveProperties != nullptr;
            }
        }

        property bool RangesUsed
        {
            bool get()
            {
                return _ranges != nullptr;
            }
        }
    };

}
