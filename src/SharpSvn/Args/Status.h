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

    /// <summary>Extended Parameter container for SvnClient.Status</summary>
    /// <threadsafety static="true" instance="false"/>
    public ref class SvnStatusArgs : public SvnClientArgs
    {
        SvnDepth _depth;
        bool _getAll;
        bool _update;
        bool _noIgnore;
        bool _ignoreExternals;
        bool _noWc;
        bool _sticky;
        SvnRevision^ _revision;
        SvnChangeListCollection^ _changelists;

    public:
        DECLARE_EVENT(SvnStatusEventArgs^, Status)

    protected public:
        virtual void OnStatus(SvnStatusEventArgs^ e)
        {
            Status(this, e);
        }

    public:
        SvnStatusArgs()
        {
            _depth = SvnDepth::Infinity;
            _revision = SvnRevision::None;
        }

        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::Status;
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

        /// <summary>Gets or sets a boolean indicating whether all status properties should be retrieved</summary>
        /// <remarks>
        /// If @a get_all is set, retrieve all entries; otherwise,
        /// retrieve only "interesting" entries (local mods and/or
        /// out of date
        /// </remarks>
        property bool RetrieveAllEntries
        {
            bool get()
            {
                return _getAll;
            }
            void set(bool value)
            {
                _getAll = value;
            }
        }

        /// <summary>Gets or sets a boolean indicating whether the repository should be contacted to retrieve out of date information</summary>
        /// <remarks>
        /// If Update is set, contact the repository and augment the
        /// status structures with information about out-of-dateness (with
        /// respect to @a revision).  Also, if @a result_rev is not @c NULL,
        /// set @a *result_rev to the actual revision against which the
        /// working copy was compared (result_rev is not meaningful unless
        /// update is set
        /// </remarks>
        property bool RetrieveRemoteStatus
        {
            bool get()
            {
                return _update;
            }
            void set(bool value)
            {
                _update = value;
            }
        }

        property bool IgnoreWorkingCopyStatus
        {
            bool get()
            {
                return _noWc;
            }
            void set(bool value)
            {
                _noWc = value;
            }
        }

        /// <summary>Gets or sets a boolean indicating whether ignored files should be retrieved</summary>
        /// <remarks>If RetrieveIgnoredEntries is set add files or directories that match ignore patterns.</remarks>
        property bool RetrieveIgnoredEntries
        {
            bool get()
            {
                return _noIgnore;
            }
            void set(bool value)
            {
                _noIgnore = value;
            }
        }

        /// <summary>Gets or sets a boolean indicating whether externals should be ignored</summary>
        /// <remarks>
        /// If IgnoreExternals is not set, then recurse into externals
        /// definitions (if any exist) after handling the main target.  This
        /// calls the client notification function (in @a ctx) with the @c
        /// svn_wc_notify_status_external action before handling each externals
        /// definition, and with @c svn_wc_notify_status_completed
        /// after each.
        /// </remarks>
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

    /// <summary>If this value is TRUE and RetrieveRemoteStatus it TRUE, shows what an update with KeepDepth TRUE would do. (Shows excluded nodes as additions)</summary>
    property bool KeepDepth
        {
            bool get()
            {
                return _sticky;
            }
            void set(bool value)
            {
                _sticky = value;
            }
        }
    };

}
