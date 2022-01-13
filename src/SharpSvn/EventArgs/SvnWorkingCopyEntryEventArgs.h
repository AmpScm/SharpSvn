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

#include "SvnLockInfo.h"
// Included from SvnClientArgs.h

namespace SharpSvn {

    public ref class SvnWorkingCopyEntryEventArgs sealed : public SvnCancelEventArgs
    {
        // This class looks remarkibly simalar to SvnWorkingCopyInfo
        // I don't use them as the same to keep both open for future extensions
        // in different directions and have quite different use cases

        initonly String^ _basePath;
        const char* _pPath;
        const svn_wc_entry_t* _entry;
        AprPool^ _pool;

        String^ _path;
        String^ _fullPath;
        String^ _name;
        initonly __int64 _rev;
        Uri^ _uri;
        Uri^ _reposUri;
        String^ _uuid;
        initonly SvnNodeKind _kind;
        initonly SvnSchedule _schedule;
        initonly bool _copied, _deleted, _absent, _incomplete;

        Uri^ _copyFrom;
        initonly __int64 _copyFromRev;

        String^ _conflictOld;
        String^ _conflictNew;
        String^ _conflictWork;
        String^ _prejfile;
        initonly DateTime _textTime;
        String^ _checksum;
        initonly __int64 _lastChangeRev;
        initonly DateTime _lastChangeTime;
        String^ _lastChangeAuthor;
        String^ _lockToken;
        String^ _lockOwner;
        String^ _lockComment;
        initonly DateTime _lockTime;
        initonly bool _hasProperties;
        initonly bool _hasPropertyChanges;
        Collection<String^>^ _cachableProperties;
        Collection<String^>^ _availableProperties;
        String^ _changelist;
        initonly __int64 _wcSize;
        initonly bool _keepLocal;
        initonly SvnDepth _depth;

    internal:
        SvnWorkingCopyEntryEventArgs(String^ basePath, const char* path, const svn_wc_entry_t* entry, AprPool^ pool)
        {
            if (String::IsNullOrEmpty(basePath))
                throw gcnew ArgumentNullException("basePath");
            else if (!path)
                throw gcnew ArgumentNullException("path");
            else if (!entry)
                throw gcnew ArgumentNullException("entry");
            else if (!pool)
                throw gcnew ArgumentNullException("pool");

            _basePath = basePath;
            _pPath = path;
            _entry = entry;
            _pool = pool;

            _rev = entry->revision;
            _kind = (SvnNodeKind)entry->kind;
            _schedule = (SvnSchedule)entry->schedule;
            _copied = (entry->copied != 0);
            _deleted = (entry->deleted != 0);
            _absent = (entry->absent != 0);
            _incomplete = (entry->incomplete != 0);
            _copyFromRev = entry->copyfrom_rev;
            _textTime = SvnBase::DateTimeFromAprTime(entry->text_time);
            _lastChangeRev = entry->cmt_rev;
            _lastChangeTime = SvnBase::DateTimeFromAprTime(entry->cmt_date);
            _lockTime = SvnBase::DateTimeFromAprTime(entry->lock_creation_date);
            _hasProperties = (entry->has_props != 0);
            _hasPropertyChanges = (entry->has_prop_mods != 0);
            _wcSize = entry->working_size;
            _keepLocal = (entry->keep_local != 0);
            _depth = (SvnDepth)entry->depth;
        }

    public:
        property String^ Path
        {
            String^ get()
            {
                if (!_path && _pPath && _pool)
                    _path = SvnBase::Utf8_PathPtrToString(_pPath, _pool);

                return _path;
            }
        }

        property String^ FullPath
        {
            String^ get()
            {
                if (!_fullPath && Path)
                    _fullPath = (Path->Length > 0) ? SvnTools::PathCombine(_basePath, Path) : _basePath;

                return _fullPath;
            }
        }

        property String^ Name
        {
            String^ get()
            {
                if (!_name && _entry)
                    _name = SvnBase::Utf8_PtrToString(_entry->name);

                return _name;
            }
        }

        property __int64 Revision
        {
            __int64 get()
            {
                return _rev;
            }
        }

        property System::Uri^ Uri
        {
            System::Uri^ get()
            {
                if (!_uri && _entry && _entry->url)
                    _uri = SvnBase::Utf8_PtrToUri(_entry->url, NodeKind);

                return _uri;
            }
        }

        property System::Uri^ RepositoryUri
        {
            System::Uri^ get()
            {
                if (!_reposUri && _entry && _entry->repos)
                    _reposUri = SvnBase::Utf8_PtrToUri(_entry->repos, SvnNodeKind::Directory);

                return _reposUri;
            }
        }

        property String^ RepositoryIdValue
        {
            String^ get()
            {
                if (!_uuid && _entry && _entry->uuid)
                    _uuid = SvnBase::Utf8_PtrToString(_entry->uuid);

                return _uuid;
            }
        }

        property Guid RepositoryId
        {
            Guid get()
            {
                if (RepositoryIdValue)
                    return Guid(_uuid);
                else
                    return Guid::Empty;
            }
        }

        property SvnNodeKind NodeKind
        {
            SvnNodeKind get()
            {
                return _kind;
            }
        }


        property SvnSchedule Schedule
        {
            SvnSchedule get()
            {
                return _schedule;
            }
        }

        property bool IsCopied
        {
            bool get()
            {
                return _copied;
            }
        }

        property bool IsDeleted
        {
            bool get()
            {
                return _deleted;
            }
        }

        property bool Absent
        {
            bool get()
            {
                return _absent;
            }
        }

        property bool Incomplete
        {
            bool get()
            {
                return _incomplete;
            }
        }

        property System::Uri^ CopiedFrom
        {
            System::Uri^ get()
            {
                if (!_copyFrom && _entry && _entry->copyfrom_url)
                    _copyFrom = SvnBase::Utf8_PtrToUri(_entry->copyfrom_url, NodeKind);

                return _copyFrom;
            }
        }

        property __int64 CopiedFromRevision
        {
            __int64 get()
            {
                return _copyFromRev;
            }
        }

        property String^ ConflictOldFile
        {
            String^ get()
            {
                if (!_conflictOld && _entry && _entry->conflict_old)
                    _conflictOld = SvnBase::Utf8_PtrToString(_entry->conflict_old)->Replace('/', System::IO::Path::DirectorySeparatorChar);

                return _conflictOld;
            }
        }

        property String^ ConflictNewFile
        {
            String^ get()
            {
                if (!_conflictNew && _entry && _entry->conflict_new)
                    _conflictNew = SvnBase::Utf8_PtrToString(_entry->conflict_new)->Replace('/', System::IO::Path::DirectorySeparatorChar);

                return _conflictNew;
            }
        }

        property String^ ConflictWorkFile
        {
            String^ get()
            {
                if (!_conflictWork && _entry && _entry->conflict_wrk)
                    _conflictWork = SvnBase::Utf8_PtrToString(_entry->conflict_wrk)->Replace('/', System::IO::Path::DirectorySeparatorChar);

                return _conflictWork;
            }
        }

        property String^ PropertyRejectFile
        {
            String^ get()
            {
                if (!_prejfile && _entry && _entry->prejfile)
                    _prejfile = SvnBase::Utf8_PtrToString(_entry->prejfile)->Replace('/', System::IO::Path::DirectorySeparatorChar);

                return _prejfile;
            }
        }

        property DateTime ContentChangeTime
        {
            DateTime get()
            {
                return _textTime;
            }
        }

        property String^ Checksum
        {
            String^ get()
            {
                if (!_checksum && _entry && _entry->checksum)
                    _checksum = SvnBase::Utf8_PtrToString(_entry->checksum);

                return _checksum;
            }
        }

        property __int64 LastChangeRevision
        {
            __int64 get()
            {
                return _lastChangeRev;
            }
        }

        property DateTime LastChangeTime
        {
            DateTime get()
            {
                return _lastChangeTime;
            }
        }

        property String^ LastChangeAuthor
        {
            String^ get()
            {
                if (!_lastChangeAuthor && _entry && _entry->cmt_author)
                    _lastChangeAuthor = SvnBase::Utf8_PtrToString(_entry->cmt_author);

                return _lastChangeAuthor;
            }
        }

        property String^ LockToken
        {
            String^ get()
            {
                if (!_lockToken && _entry && _entry->lock_token)
                    _lockToken = SvnBase::Utf8_PtrToString(_entry->lock_token);

                return _lockToken;
            }
        }

        property String^ LockOwner
        {
            String^ get()
            {
                if (!_lockOwner && _entry && _entry->lock_owner)
                    _lockOwner = SvnBase::Utf8_PtrToString(_entry->lock_owner);

                return _lockOwner;
            }
        }

        property String^ LockComment
        {
            String^ get()
            {
                if (!_lockComment && _entry && _entry->lock_comment)
                    _lockComment = SvnBase::Utf8_PtrToString(_entry->lock_comment);

                return _lockOwner;
            }
        }

        property DateTime LockTime
        {
            DateTime get()
            {
                return _lockTime;
            }
        }

        property bool HasProperties
        {
            bool get()
            {
                return _hasProperties;
            }
        }

        property bool HasPropertyChanges
        {
            bool get()
            {
                return _hasPropertyChanges;
            }
        }

        property Collection<String^>^ CacheableProperties
        {
            Collection<String^>^ get()
            {
                if (!_cachableProperties && _entry && _entry->cachable_props)
                {
                    Collection<String^>^ items = gcnew Collection<String^>();

                    for each (String^ i in SvnBase::Utf8_PtrToString(_entry->cachable_props)->Split(' '))
                    {
                        if (!String::IsNullOrEmpty(i))
                            items->Add(i);
                    }

                    _cachableProperties = items;
                }
                return _cachableProperties;
            }
        }

        property Collection<String^>^ AvailableProperties
        {
            Collection<String^>^ get()
            {
                if (!_availableProperties && _entry && _entry->present_props)
                {
                    Collection<String^>^ items = gcnew Collection<String^>();

                    for each (String^ i in SvnBase::Utf8_PtrToString(_entry->present_props)->Split(' '))
                    {
                        if (!String::IsNullOrEmpty(i))
                            items->Add(i);
                    }

                    _availableProperties = items;
                }
                return _availableProperties;
            }
        }

        property String^ ChangeList
        {
            String^ get()
            {
                if (!_changelist && _entry && _entry->changelist)
                    _changelist = SvnBase::Utf8_PtrToString(_entry->changelist);

                return _changelist;
            }
        }

        property __int64 WorkingCopySize
        {
            __int64 get()
            {
                return _wcSize;
            }
        }

        property bool KeepLocal
        {
            bool get()
            {
                return _keepLocal;
            }
        }

        property SvnDepth Depth
        {
            SvnDepth get()
            {
                return _depth;
            }
        }


    protected public:
        virtual void Detach(bool keepProperties) override
        {
            try
            {
                if (keepProperties)
                {
                    GC::KeepAlive(Path);
                    GC::KeepAlive(FullPath);
                    GC::KeepAlive(Name);
                    GC::KeepAlive(Uri);
                    GC::KeepAlive(RepositoryUri);
                    GC::KeepAlive(RepositoryIdValue);
                    GC::KeepAlive(CopiedFrom);
                    GC::KeepAlive(ConflictOldFile);
                    GC::KeepAlive(ConflictNewFile);
                    GC::KeepAlive(ConflictWorkFile);
                    GC::KeepAlive(PropertyRejectFile);
                    GC::KeepAlive(Checksum);
                    GC::KeepAlive(LastChangeAuthor);
                    GC::KeepAlive(LockToken);
                    GC::KeepAlive(LockOwner);
                    GC::KeepAlive(LockComment);
                    GC::KeepAlive(CacheableProperties);
                    GC::KeepAlive(AvailableProperties);
                    GC::KeepAlive(ChangeList);
                }
            }
            finally
            {
                _pool = nullptr;
                _pPath = nullptr;
                _entry = nullptr;
                __super::Detach(keepProperties);
            }
        }
    };
};
