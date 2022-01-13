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

// Included from SvnClientArgs.h

namespace SharpSvn {

    public ref class SvnChangeItem sealed
    {
        const svn_log_changed_path2_t* _changed_path;
        initonly String^ _path;
        initonly SvnChangeAction _action;
        String^ _copyFromPath;
        initonly __int64 _copyFromRevision;
        initonly SvnNodeKind _nodeKind;
        Uri^ _repositoryPath;
        initonly System::Nullable<bool> _textModified;
        initonly System::Nullable<bool> _propsModified;

    internal:
        SvnChangeItem(String^ path, const svn_log_changed_path2_t* changed_path)
        {
            if (String::IsNullOrEmpty(path))
                throw gcnew ArgumentNullException("path");
            else if (!changed_path)
                throw gcnew ArgumentNullException("changed_path");

            _changed_path = changed_path;
            _path = path;
            _action = (SvnChangeAction)changed_path->action;
            _copyFromPath = SvnBase::Utf8_PtrToString(changed_path->copyfrom_path);
            _copyFromRevision = changed_path->copyfrom_path ? changed_path->copyfrom_rev : -1;
            _nodeKind = (SvnNodeKind)changed_path->node_kind;

            switch (changed_path->text_modified)
            {
                case svn_tristate_false:
                    _textModified = false;
                    break;
                case svn_tristate_true:
                    _textModified = true;
                    break;
                default:
                    ;
            }

            switch (changed_path->props_modified)
            {
            case svn_tristate_false:
                _propsModified = false;
                break;
            case svn_tristate_true:
                _propsModified = true;
                break;
            default:
                ;
            }
        }

    public:
        /// <summary>Gets the path inside rooted at the repository root (including initial '/')</summary>
        property String^ Path
        {
            String^ get()
            {
                return _path;
            }
        }

        /// <summary>Gets the relative uri of the path inside the repository</summary>
        /// <remarks>Does not include an initial '/'. Ends with a '/' if <see cref="NodeKind" /> is <see cref="SvnNodeKind::Directory" />.</remarks>
        property Uri^ RepositoryPath
        {
            Uri^ get()
            {
                if (!_repositoryPath && _path)
                    Uri::TryCreate(_path->Substring(1) + ((NodeKind == SvnNodeKind::Directory) ? "/" : ""), UriKind::Relative, _repositoryPath);

                return _repositoryPath;
            }
        }

        property SvnChangeAction Action
        {
            SvnChangeAction get()
            {
                return _action;
            }
        }

        property String^ CopyFromPath
        {
            String^ get()
            {
                if (!_copyFromPath && _changed_path && _changed_path->copyfrom_path)
                    _copyFromPath = SvnBase::Utf8_PtrToString(_changed_path->copyfrom_path);

                return _copyFromPath;
            }
        }

        property __int64 CopyFromRevision
        {
            __int64 get()
            {
                return _copyFromRevision;
            }
        }

        /// <summary>Gets the node kind of the changed path (Only available when committed to a 1.6+ repository)</summary>
        property SvnNodeKind NodeKind
        {
            SvnNodeKind get()
            {
                return _nodeKind;
            }
        }

        /// <summary>Gets a boolean indicating whether the content of a node is modified in this revision. (Value only available for 1.7+ servers)</summary>
        property System::Nullable<bool> ContentModified
        {
            System::Nullable<bool> get()
            {
                return _textModified;
            }
        }

        /// <summary>Gets a boolean indicating whether the versioned properties of a node are modified in this revision. (Value only available for 1.7+ servers)</summary>
        property System::Nullable<bool> PropertiesModified
        {
            System::Nullable<bool> get()
            {
                return _propsModified;
            }
        }

        /// <summary>Serves as a hashcode for the specified type</summary>
        virtual int GetHashCode() override
        {
            return CopyFromRevision.GetHashCode() ^ Path->GetHashCode();
        }

        void Detach()
        {
            Detach(true);
        }

    protected public:
        virtual void Detach(bool keepProperties)
        {
            try
            {
                if (keepProperties)
                {
                    GC::KeepAlive(CopyFromPath);
                }
            }
            finally
            {
                _changed_path = nullptr;
            }
        }
    };

    namespace Implementation {
        public ref class SvnChangeItemCollection sealed : System::Collections::ObjectModel::KeyedCollection<String^, SvnChangeItem^>
        {
        internal:
            SvnChangeItemCollection()
            {}
        protected:
            virtual String^ GetKeyForItem(SvnChangeItem^ item) override
            {
                if (!item)
                    throw gcnew ArgumentNullException("item");

                return item->Path;
            }
        };
    }

    public ref class SvnLoggingEventArgs abstract : public SvnCancelEventArgs
    {
        const svn_log_entry_t* _entry;
        AprPool^ _pool;

        String^ _author;
        initonly DateTime _date;
        String^ _message;
        initonly __int64 _revision;
        initonly bool _nonInheritable;
        initonly bool _subtractiveMerge;

        const char* _pcAuthor;
        const char* _pcMessage;
        SvnPropertyCollection^ _customProperties;

        SvnChangeItemCollection^ _changedPaths;
        array<SvnChangeItem^>^ _changeItemsToDetach;

    internal:
        SvnLoggingEventArgs(const svn_log_entry_t *entry, AprPool ^pool)
        {
            if (!entry)
                throw gcnew ArgumentNullException("entry");
            else if (!pool)
                throw gcnew ArgumentNullException("pool");

            _entry = entry;
            _pool = pool;

            const char* pcAuthor = nullptr;
            const char* pcDate = nullptr;
            const char* pcMessage = nullptr;

            if (entry->revprops)
                svn_compat_log_revprops_out(&pcAuthor, &pcDate, &pcMessage, entry->revprops);

            if (pcDate)
            {
                apr_time_t when = 0; // Documentation: date must be parsable by svn_time_from_cstring()
                svn_error_t *err = pcDate ? svn_time_from_cstring(&when, pcDate, pool->Handle) : nullptr;

                if (!err)
                    _date = SvnBase::DateTimeFromAprTime(when);
                else
                    svn_error_clear(err);
            }
            else
                _date = DateTime::MinValue;

            _revision = entry->revision;
            _pcAuthor = pcAuthor;
            _pcMessage = pcMessage;

            _nonInheritable = (entry->non_inheritable != FALSE);
            _subtractiveMerge = (entry->subtractive_merge != FALSE);
        }

    public:

        property SvnChangeItemCollection^ ChangedPaths
        {
            SvnChangeItemCollection^ get();
        }

        /// <summary>Gets the list of custom properties retrieved with the log</summary>
        /// <remarks>Properties must be listed in SvnLogArgs.RetrieveProperties to be available here</remarks>
        property SvnPropertyCollection^ RevisionProperties
        {
            SvnPropertyCollection^ get()
            {
                if (!_customProperties && _entry && _entry->revprops && _pool)
                {
                    _customProperties = SvnBase::CreatePropertyDictionary(_entry->revprops, _pool);
                }
                return _customProperties;
            }
        }

        property __int64 Revision
        {
            __int64 get()
            {
                return _revision;
            }
        }

        property String^ Author
        {
            String^ get()
            {
                if (!_author && _pcAuthor)
                    _author = SvnBase::Utf8_PtrToString(_pcAuthor);

                return _author;
            }
        }

        property DateTime Time
        {
            DateTime get()
            {

                return _date;
            }
        }

        property String^ LogMessage
        {
            String^ get()
            {
                if (!_message && _pcMessage)
                {
                    _message = SvnBase::Utf8_PtrToString(_pcMessage);

                    if (_message)
                    {
                        // Subversion log messages always use \n newlines
                        _message = _message->Replace("\n", Environment::NewLine);
                    }
                }

                return _message;
            }
        }

    /// <summary>MergeInfo only: Not inheritable</summary>
        property bool NotInheritable
        {
            bool get()
            {
                return _nonInheritable;
            }
        }

        /// <summary>MergeInfo only: Subtractive merge</summary>
        property bool SubtractiveMerge
        {
            bool get()
            {
                return _subtractiveMerge;
            }
        }


        /// <summary>Serves as a hashcode for the specified type</summary>
        virtual int GetHashCode() override
        {
            return Revision.GetHashCode();
        }

    protected public:
        virtual void Detach(bool keepProperties) override
        {
            try
            {
                if (keepProperties)
                {
                    // Use all properties to get them cached in .Net memory
                    GC::KeepAlive(ChangedPaths);
                    GC::KeepAlive(Author);
                    GC::KeepAlive(LogMessage);
                    GC::KeepAlive(RevisionProperties);
                }

                if (_changeItemsToDetach)
                {
                    for each (SvnChangeItem^ i in _changeItemsToDetach)
                    {
                        i->Detach(keepProperties);
                    }
                }
            }
            finally
            {
                _entry = nullptr;
                _pool = nullptr;
                _pcMessage = nullptr;
                _pcAuthor = nullptr;
                _changeItemsToDetach = nullptr;
                __super::Detach(keepProperties);
            }
        }
    };


    public ref class SvnLogEventArgs : public SvnLoggingEventArgs
    {
        initonly Uri^ _logOrigin;
        initonly bool _hasChildren;
        initonly int _mergeLevel;

    internal:
        SvnLogEventArgs(const svn_log_entry_t *entry, int mergeLevel, Uri^ logOrigin, AprPool ^pool)
            : SvnLoggingEventArgs(entry, pool)
        {
            _hasChildren = entry->has_children ? true : false;
            _mergeLevel = mergeLevel;
            _logOrigin = logOrigin;
        }

    public:
                /// <summary>Gets the log origin SharpSvn used for retrieving the log</summary>
        property Uri^ LogOrigin
        {
            Uri^ get()
            {
                return _logOrigin;
            }
        }

        /// <summary>Set to true when the following items are merged-child items of this item.</summary>
        property bool HasChildren
        {
            bool get()
            {
                return _hasChildren;
            }
        }

        /// <summary>Gets the nesting level of the logs via merges</summary>
        property int MergeLogNestingLevel
        {
            int get()
            {
                return _mergeLevel;
            }
        }
    };

}
