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

    public ref class SvnImportFilterEventArgs : public SvnCancelEventArgs
    {
        initonly String^ _fullPath;
        initonly SvnNodeKind _kind;
        initonly bool _special;
        bool _filter;

    internal:
        SvnImportFilterEventArgs(const char *local_abspath, const svn_io_dirent2_t *dirent, AprPool^ pool)
        {
            if (!local_abspath)
                throw gcnew ArgumentNullException("local_abspath");
            else if (!dirent)
                throw gcnew ArgumentNullException("dirent");

            _fullPath = SvnBase::Utf8_PathPtrToString(local_abspath, pool);
            _kind = (SvnNodeKind)dirent->kind;
            _special = (0 != dirent->special);
        }

    public:
        property String^ FullPath
		{
			String^ get()
			{
				return _fullPath;
			}
		}

        property SvnNodeKind NodeKind
        {
            SvnNodeKind get()
            {
                return _kind;
            }
        }

        property bool IsSymlink
        {
            bool get()
            {
                return _special;
            }
        }

        /// <summary>True to filter this node, false to allow importing it</summary>
        property bool Filter
        {
            bool get()
            {
                return _filter;
            }
            void set(bool value)
            {
                _filter = value;
            }
        }
    };
}
