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

#include "AprArray.h"

namespace SharpSvn {
    using namespace SharpSvn::Implementation;
    using System::Collections::Generic::ICollection;
    using System::Collections::Generic::IList;

    ref class SvnCommitItem;
    ref class SvnClient;
    ref class SvnClientArgs;

    public ref class SvnCommitResult : public SvnCommandResult
    {
        initonly __int64 _revision;
        initonly DateTime _date;
        initonly String^ _author;
        initonly String^ _postCommitError;
        initonly Uri^ _reposRoot;

    private protected:
    SvnCommitResult(const svn_commit_info_t *commitInfo, AprPool^ pool);

    public:
        property __int64 Revision
        {
            __int64 get()
            {
                return _revision;
            }
        }

        property DateTime Time
        {
            DateTime get()
            {
                return _date;
            }
        }

        property String^ Author
        {
            String^ get()
            {
                return _author;
            }
        }

        property Uri^ RepositoryRoot
        {
            Uri^ get()
            {
                return _reposRoot;
            }
        }

        /// <summary>error message from post-commit hook, or NULL</summary>
        property String^ PostCommitError
        {
            String^ get()
            {
                return _postCommitError;
            }
        }

        /// <summary>Serves as a hashcode for the specified type</summary>
        virtual int GetHashCode() override
        {
            return Revision.GetHashCode();
        }
    };

    public ref class SvnCommittedEventArgs sealed : SvnCommitResult
    {
    internal:
        static SvnCommittedEventArgs^ Create(SvnClientContext^ client, const svn_commit_info_t *commitInfo, AprPool^ pool);

    private:
        SvnCommittedEventArgs(const svn_commit_info_t *commitInfo, AprPool^ pool);

    public:

    };

    [Flags]
    public enum class SvnCommitTypes
    {
        None = 0,
        Added = SVN_CLIENT_COMMIT_ITEM_ADD,
        Deleted = SVN_CLIENT_COMMIT_ITEM_DELETE,
        ContentModified = SVN_CLIENT_COMMIT_ITEM_TEXT_MODS,
        PropertiesModified = SVN_CLIENT_COMMIT_ITEM_PROP_MODS,
        Copied = SVN_CLIENT_COMMIT_ITEM_IS_COPY,
        HasLockToken = SVN_CLIENT_COMMIT_ITEM_LOCK_TOKEN,
        MovedHere = SVN_CLIENT_COMMIT_ITEM_MOVED_HERE,
    };

    public ref class SvnCommitItem sealed
    {
        const svn_client_commit_item3_t *_info;
        AprPool^ _pool;
        String^ _path;
        String^ _fullPath;
        initonly SvnNodeKind _nodeKind;
        Uri^ _uri;
        initonly __int64 _revision;
        Uri^ _copyFromUri;
        initonly __int64 _copyFromRevision;
        initonly SvnCommitTypes _commitType;
        String^ _movedFrom;

    internal:
        SvnCommitItem(const svn_client_commit_item3_t *commitItemInfo, AprPool^ pool);

    public:
        property String^ Path
        {
            String^ get()
            {
                if (!_path && _info && _pool)
                    _path = SvnBase::Utf8_PathPtrToString(_info->path, _pool);
                return _path;
            }
        }

        property String^ FullPath
        {
            String^ get()
            {
                if (!_fullPath && Path)
                    _fullPath = SvnTools::GetNormalizedFullPath(Path);

                return _fullPath;
            }
        }

        property String^ MovedFrom
        {
            String^ get()
            {
                if (!_movedFrom && _info && _info->moved_from_abspath && _pool)
                    _movedFrom = SvnBase::Utf8_PathPtrToString(_info->moved_from_abspath, _pool);
                return _movedFrom;
            }
        }

        property SvnNodeKind NodeKind
        {
            SvnNodeKind get()
            {
                return _nodeKind;
            }
        }

        property System::Uri^ Uri
        {
            System::Uri^ get()
            {
                if (!_uri && _info && _info->url)
                    _uri = SvnBase::Utf8_PtrToUri(_info->url, _nodeKind);

                return _uri;
            }
        }

        property __int64 Revision
        {
            __int64 get()
            {
                return _revision;
            }
        }

        property System::Uri^ CopyFromUri
        {
            System::Uri^ get()
            {
                if (!_copyFromUri && _info && _info->copyfrom_url)
                    _copyFromUri = SvnBase::Utf8_PtrToUri(_info->copyfrom_url, _nodeKind);

                return _copyFromUri;
            }
        }

        property __int64 CopyFromRevision
        {
            __int64 get()
            {
                return _copyFromRevision;
            }
        }

        property SvnCommitTypes CommitType
        {
            SvnCommitTypes get()
            {
                return _commitType;
            }
        }

        /// <summary>Serves as a hashcode for the specified type</summary>
        virtual int GetHashCode() override
        {
            return Revision.GetHashCode();
        }

        void Detach()
        {
            Detach(true);
        }

    protected public:
        void Detach(bool keepProperties);
    };

    namespace Implementation {
        public ref class SvnCommitItemCollection sealed : public System::Collections::ObjectModel::KeyedCollection<String^, SvnCommitItem^>
        {
        internal:
            SvnCommitItemCollection(IList<SvnCommitItem^>^ items)
            {
                for each (SvnCommitItem^ i in items)
                {
                    Add(i);
                }
            }

        protected:
            virtual String^ GetKeyForItem(SvnCommitItem^ item) override
            {
                if (!item)
                    throw gcnew ArgumentNullException("item");

                return item->Path;
            }
        };

    }
}
