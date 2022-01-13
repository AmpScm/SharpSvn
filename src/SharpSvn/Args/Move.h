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

    /// <summary>Extended Parameter container of <see cref="SvnClient::Move(String^,String^,SvnMoveArgs^)" /> and
    /// <see cref="SvnClient::RemoteMove(Uri^,Uri^,SvnMoveArgs^)" /></summary>
    /// <threadsafety static="true" instance="false"/>
    public ref class SvnMoveArgs : public SvnClientArgsWithCommit
    {
        bool _force;
        bool _moveAsChild;
        bool _makeParents;
        bool _noMixedRevisions;
        bool _metadataOnly;

    public:
        SvnMoveArgs()
        {
        }

        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::Move;
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

        property bool AlwaysMoveAsChild
        {
            bool get()
            {
                return _moveAsChild;
            }
            void set(bool value)
            {
                _moveAsChild = value;
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

        property bool AllowMixedRevisions
        {
            bool get()
            {
                return !_noMixedRevisions;
            }
            void set(bool value)
            {
                _noMixedRevisions = !value;
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
    };

}
