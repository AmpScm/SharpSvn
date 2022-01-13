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

#include "SvnConflictData.h"

namespace SharpSvn {

    public ref class SvnInfoEventArgs : public SvnCancelEventArgs
    {
        const svn_client_info2_t* _info;
        AprPool^ _pool;

        initonly String^ _path;
        String^ _fullPath;
        Uri^ _uri;
        initonly __int64 _rev;
        initonly SvnNodeKind _nodeKind;
        Uri^ _reposRootUri;
        String^ _reposUuid;
        initonly __int64 _lastChangeRev;
        initonly DateTime _lastChangeDate;
        String^ _lastChangeAuthor;
        SvnLockInfo^ _lock;
        initonly bool _hasWcInfo;
        SvnSchedule _wcSchedule;
        Uri^ _copyFromUri;
        initonly __int64 _copyFromRev;
        initonly DateTime _contentTime;
        String^ _checksum;
        String^ _conflict_old;
        String^ _conflict_new;
        String^ _conflict_wrk;
        String^ _prejfile;
        String^ _changelist;
        initonly SvnDepth _depth;
        initonly __int64 _wcSize;
        initonly __int64 _size;
        initonly bool _conflicted;
        ICollection<SvnConflictData^>^ _conflicts;
        String^ _wcAbspath;
        String^ _movedFromAbspath;
        String^ _movedToAbspath;

    internal:
        SvnInfoEventArgs(String^ path, const svn_client_info2_t* info, AprPool^ pool)
        {
            if (path == nullptr)
                throw gcnew ArgumentNullException("path");
            else if (!info)
                throw gcnew ArgumentNullException("info");
            else if (!pool)
                throw gcnew ArgumentNullException("pool");

            _info = info;
            _pool = pool;

            _path = path;
            _rev = info->rev;
            _nodeKind = (SvnNodeKind)info->kind;
            _lastChangeRev = info->last_changed_rev;
            _lastChangeDate = SvnBase::DateTimeFromAprTime(info->last_changed_date);
            _hasWcInfo = (info->wc_info != nullptr);
            if (info->wc_info)
            {
                _wcSchedule = (SvnSchedule)info->wc_info->schedule;
                _copyFromRev = info->wc_info->copyfrom_rev;
                _depth = (SvnDepth)info->wc_info->depth;

                _contentTime = SvnBase::DateTimeFromAprTime(info->wc_info->recorded_time);
                if (info->wc_info->recorded_size == SVN_INVALID_FILESIZE)
                    _wcSize = -1;
                else
                    _wcSize = info->wc_info->recorded_size;

                _conflicted = info->wc_info->conflicts && (info->wc_info->conflicts->nelts > 0);
            }
            else
            {
                _depth = SvnDepth::Unknown;
                _wcSize = -1;
                _copyFromRev = -1;
            }

            if (info->size == SVN_INVALID_FILESIZE)
                _size = -1;
            else
                _size = info->size;
        }
    public:
        /// <summary>Gets the path of the file. The local path if requisting WORKING version, otherwise the name of the file
        /// at the specified version</summary>
        property String^ Path
        {
            String^ get()
            {
                return _path;
            }
        }

        /// <summary>The path the notification is about, translated via <see cref="SvnTools::GetNormalizedFullPath" /></summary>
        /// <remarks>The <see cref="FullPath" /> property contains the path in normalized format; while <see cref="Path" /> returns the exact path from the subversion api</remarks>
        property String^ FullPath
        {
            String^ get()
            {
                if (!_fullPath && Path && HasLocalInfo)
                    _fullPath = SvnTools::GetNormalizedFullPath(Path);

                return _fullPath;
            }
        }

        /// <summary>Gets the full Uri of the node</summary>
        property Uri^ Uri
        {
            System::Uri^ get()
            {
                if (!_uri && _info && _info->URL)
                    _uri = SvnBase::Utf8_PtrToUri(_info->URL, _nodeKind);

                return _uri;
            }
        }

        /// <summary>Gets the queried revision</summary>
        property __int64 Revision
        {
            __int64 get()
            {
                return _rev;
            }
        }

        /// <summary>Gets the node kind of the specified node</summary>
        property SvnNodeKind NodeKind
        {
            SvnNodeKind get()
            {
                return _nodeKind;
            }
        }

        /// <summary>Gets the repository root Uri; ending in a '/'</summary>
        /// <remarks>The unmanaged api does not add the '/' at the end, but this makes using <see cref="System::Uri" /> hard</remarks>
        property System::Uri^ RepositoryRoot
        {
            System::Uri^ get()
            {
                if (!_reposRootUri && _info && _info->repos_root_URL)
                    _reposRootUri = SvnBase::Utf8_PtrToUri(_info->repos_root_URL, SvnNodeKind::Directory);

                return _reposRootUri;
            }
        }

        /// <summary>Gets the uuid of the repository (if available). Otherwise Guid.Empty</summary>
        property Guid RepositoryId
        {
            Guid get()
            {
                return RepositoryIdValue ? Guid(RepositoryIdValue) : Guid::Empty;
            }
        }

        /// <summary>Gets the repository uuid or <c>null</c> if not available</summary>
        property String^ RepositoryIdValue
        {
            String^ get()
            {
                if (!_reposUuid && _info && _info->repos_UUID)
                    _reposUuid = SvnBase::Utf8_PtrToString(_info->repos_UUID);

                return _reposUuid;
            }
        }

        /// <summary>Gets the last revision in which node (or one of its descendants) changed</summary>
        property __int64 LastChangeRevision
        {
            __int64 get()
            {
                return _lastChangeRev;
            }
        }

        /// <summary>Gets the timestamp of the last revision in which node (or one of its descendants) changed</summary>
        property DateTime LastChangeTime
        {
            DateTime get()
            {
                return _lastChangeDate;
            }
        }

        /// <summary>Gets the author of the last revision in which node (or one of its descendants) changed</summary>
        property String^ LastChangeAuthor
        {
            String^ get()
            {
                if (!_lastChangeAuthor && _info && _info->last_changed_author)
                    _lastChangeAuthor = SvnBase::Utf8_PtrToString(_info->last_changed_author);

                return _lastChangeAuthor;
            }
        }

        /// <summary>Gets information about the current lock on node</summary>
        property SvnLockInfo^ Lock
        {
            SvnLockInfo^ get()
            {
                if (!_lock && _info && _info->lock && _info->lock->token)
                    _lock = gcnew SvnLockInfo(_info->lock, HasLocalInfo);

                return _lock;
            }
        }

        /// <summary>Gets a boolean indicating whether working copy information is available in this instance</summary>
        property bool HasLocalInfo
        {
            bool get()
            {
                return _hasWcInfo;
            }
        }

        property SvnSchedule Schedule
        {
            SvnSchedule get()
            {
                return HasLocalInfo ? _wcSchedule : SvnSchedule::Normal;
            }
        }

        property System::Uri^ CopyFromUri
        {
            System::Uri^ get()
            {
                if (!_copyFromUri && _info && _info->wc_info && _info->wc_info->copyfrom_url)
                    _copyFromUri = SvnBase::Utf8_PtrToUri(_info->wc_info->copyfrom_url, _nodeKind);

                return _copyFromUri;
            }
        }

        property __int64 CopyFromRevision
        {
            __int64 get()
            {
                return _copyFromRev;
            }
        }

        property DateTime ContentTime
        {
            DateTime get()
            {
                return _contentTime;
            }
        }

        /// <summary>The SHA1 checksum of the file. (Used to return a MD5 checksom in Subversion &lt;= 1.6)</summary>
        property String^ Checksum
        {
            String^ get()
            {
                if (!_checksum && _info && _info->wc_info && _info->wc_info->checksum && _pool)
                    _checksum = SvnBase::Utf8_PtrToString(svn_checksum_to_cstring(_info->wc_info->checksum, _pool->Handle));

                return _checksum;
            }
        }

        property ICollection<SvnConflictData^>^ Conflicts
        {
            ICollection<SvnConflictData^>^ get();
        }

        property String^ ConflictOld
        {
            String^ get()
            {
                if (Conflicted && Conflicts)
                    for each (SvnConflictData ^ d in Conflicts)
                    {
                        if (d->ConflictType == SvnConflictType::Content)
                            return d->BaseFile;
                    }

                return nullptr;
            }
        }

        property String^ ConflictNew
        {
            String^ get()
            {
                if (Conflicted && Conflicts)
                    for each (SvnConflictData ^ d in Conflicts)
                    {
                        if (d->ConflictType == SvnConflictType::Content)
                            return d->TheirFile;
                    }

                return nullptr;
            }
        }

        property String^ ConflictWork
        {
            String^ get()
            {
                if (Conflicted && Conflicts)
                    for each (SvnConflictData ^ d in Conflicts)
                    {
                        if (d->ConflictType == SvnConflictType::Content)
                            return d->MyFile;
                    }

                return nullptr;
            }
        }

        property String^ PropertyEditFile
        {
            String^ get()
            {
                if (Conflicted && Conflicts)
                    for each (SvnConflictData ^ d in Conflicts)
                    {
                        if (d->ConflictType == SvnConflictType::Property)
                            return d->TheirFile;
                    }

                return nullptr;
            }
        }

        property SvnDepth Depth
        {
            SvnDepth get()
            {
                return _depth;
            }
        }

        property String^ ChangeList
        {
            String^ get()
            {
                if (!_changelist && _info && _info->wc_info && _info->wc_info->changelist)
                    _changelist = SvnBase::Utf8_PtrToString(_info->wc_info->changelist);

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

        property __int64 RepositorySize
        {
            __int64 get()
            {
                return _size;
            }
        }

        property SvnConflictData^ TreeConflict
        {
            SvnConflictData^ get()
            {
                if (Conflicted && Conflicts)
                    for each (SvnConflictData ^ d in Conflicts)
                    {
                        if (d->ConflictType == SvnConflictType::Tree)
                            return d;
                    }

                return nullptr;
            }
        }

        property bool Conflicted
        {
            bool get()
            {
                return _conflicted;
            }
        }

        property String^ WorkingCopyRoot
        {
            String^ get()
            {
                if (!_wcAbspath && _info && _info->wc_info && _pool)
                    _wcAbspath = SvnBase::Utf8_PathPtrToString(_info->wc_info->wcroot_abspath, _pool);

                return _wcAbspath;
            }
        }

        property String^ MovedFrom
        {
            String^ get()
            {
                if (!_movedFromAbspath && _info && _info->wc_info && _info->wc_info->moved_from_abspath)
                    _movedFromAbspath = SvnBase::Utf8_PathPtrToString(_info->wc_info->moved_from_abspath, _pool);

                return _movedFromAbspath;
            }
        }

        property String^ MovedTo
        {
            String^ get()
            {
                if (!_movedToAbspath && _info && _info->wc_info && _info->wc_info->moved_to_abspath)
                    _movedToAbspath = SvnBase::Utf8_PathPtrToString(_info->wc_info->moved_to_abspath, _pool);

                return _movedToAbspath;
            }
        }

        /// <summary>Serves as a hashcode for the specified type</summary>
        virtual int GetHashCode() override
        {
            return SafeGetHashCode(Path) ^ SafeGetHashCode(Uri);
        }

    protected public:
        virtual void Detach(bool keepProperties) override
        {
            try
            {
                if (keepProperties)
                {
                    // Use all properties to get them cached in .Net memory
                    GC::KeepAlive(Path);
                    GC::KeepAlive(Uri);
                    GC::KeepAlive(RepositoryRoot);
                    GC::KeepAlive(RepositoryId);
                    GC::KeepAlive(LastChangeAuthor);
                    GC::KeepAlive(Lock);
                    GC::KeepAlive(CopyFromUri);
                    GC::KeepAlive(Checksum);
                    GC::KeepAlive(ConflictOld);
                    GC::KeepAlive(ConflictNew);
                    GC::KeepAlive(ConflictWork);
                    GC::KeepAlive(PropertyEditFile);
                    GC::KeepAlive(ChangeList);
                    GC::KeepAlive(Conflicts);
                    GC::KeepAlive(WorkingCopyRoot);
                    GC::KeepAlive(MovedFrom);
                    GC::KeepAlive(MovedTo);
                }

                if (_conflicts)
                    for each (SvnConflictData ^ cd in _conflicts)
                        cd->Detach(keepProperties);

                if (_lock)
                    _lock->Detach(keepProperties);
            }
            finally
            {
                _info = nullptr;
                _pool = nullptr;
                __super::Detach(keepProperties);
            }
        }
    };

}
