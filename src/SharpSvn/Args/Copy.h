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

    /// <summary>Extended Parameter container of SvnClient.Copy(SvnTarget^,String^,SvnCopyArgs^)" /> and
    /// <see cref="SvnClient::RemoteCopy(SvnTarget^,Uri^,SvnCopyArgs^)" /></summary>
    /// <threadsafety static="true" instance="false"/>
    public ref class SvnCopyArgs : public SvnClientArgsWithCommit
    {
        bool _makeParents;
        bool _alwaysCopyBelow;
        bool _ignoreExternals;
        bool _metadataOnly;
        bool _pinExternals;
        SvnRevision^ _revision;

    public:
        SvnCopyArgs()
        {
            _revision = SvnRevision::None;
        }

        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::Copy;
            }
        }

        /// <summary>Creates parent directories if required</summary>
        property bool CreateParents
        {
            bool get()
            {
                return _makeParents;
            }
            void set(bool value)
            {
                _makeParents = value;
            }
        }

        /// <summary>Always copies the result to below the target (this behaviour is always used if multiple targets are provided)</summary>
        property bool AlwaysCopyAsChild
        {
            bool get()
            {
                return _alwaysCopyBelow;
            }
            void set(bool value)
            {
                _alwaysCopyBelow = value;
            }
        }

        property bool MetaDataOnly
        {
            bool get()
            {
                return _metadataOnly;
            }
            void set(bool value)
            {
                _metadataOnly = value;
            }
        }

        property bool PinExternals
        {
            bool get()
            {
                return _pinExternals;
            }
            void set(bool value)
            {
                _pinExternals = value;
            }
        }

        /// <summary>Gets or sets a boolean that if set to true tells copy not to process
        /// externals definitions as part of this operation.</summary>
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

        property SvnRevision^ Revision
        {
            SvnRevision^ get()
            {
                return _revision;
            }
            void set(SvnRevision^ value)
            {
                if (value)
                    _revision = value;
                else
                    _revision = SvnRevision::None;
            }
        }
    };

}
