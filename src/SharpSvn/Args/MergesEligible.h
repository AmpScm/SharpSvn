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

#include "Log.h"

namespace SharpSvn {

    /// <threadsafety static="true" instance="false"/>
    public ref class SvnMergesEligibleArgs : public SvnClientArgs
    {
        SvnRevisionPropertyNameCollection^ _retrieveProperties;
        bool _logChangedPaths;
        SvnDepth _depth;
        SvnRevisionRange ^_range;
    internal:
        SvnTarget^ _sourceTarget;

    public:
        SvnMergesEligibleArgs()
        {
            _depth = SvnDepth::Infinity;
            _range = SvnRevisionRange::None;
        }

        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::MergesEligible;
            }
        }

    public:
        DECLARE_EVENT(SvnMergesEligibleEventArgs^, MergesEligible);

    protected public:
        virtual void OnMergesEligible(SvnMergesEligibleEventArgs^ e)
        {
            MergesEligible(this, e);
        }

    public:
        /// <summary>Gets or sets a boolean indicating whether the paths changed in the revision should be provided</summary>
        property bool RetrieveChangedPaths
        {
            bool get()
            {
                return _logChangedPaths;
            }
            void set(bool value)
            {
                _logChangedPaths = value;
            }
        }

        /// <summary>Gets the list of properties to retrieve. Only SVN 1.5+ repositories allow adding custom properties to this list</summary>
        property SvnRevisionPropertyNameCollection^ RetrieveProperties
        {
            SvnRevisionPropertyNameCollection^ get()
            {
                if (!_retrieveProperties)
                    _retrieveProperties = gcnew SvnRevisionPropertyNameCollection(false);

                return _retrieveProperties;
            }
        }

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

        property SvnRevisionRange^ Range
        {
            SvnRevisionRange^ get()
            {
                return _range;
            }
            void set(SvnRevisionRange ^value)
            {
                if (!value)
                    _range = SvnRevisionRange::None;
                else
                    _range = value;
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
    };

}
