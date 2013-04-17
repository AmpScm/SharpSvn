// $Id$
//
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

	/// <summary>Extended Parameter container of <see cref="SvnRepositoryClient" />'s CreateRepository method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnCreateRepositoryArgs : public SvnClientArgs
	{
		bool _bdbNoFSyncCommit;
		bool _bdbKeepLogs;
		SvnRepositoryFileSystem _reposType;
		SvnRepositoryCompatibility _reposCompat;
        Nullable<System::Guid> _uuid;

	public:
		SvnCreateRepositoryArgs()
		{
		}

		property bool BerkeleyDBNoFSyncAtCommit
		{
			bool get()
			{
				return _bdbNoFSyncCommit;
			}
			void set(bool value)
			{
				_bdbNoFSyncCommit = value;
			}
		}

		virtual property SvnCommandType CommandType
		{
			virtual SvnCommandType get() override sealed
			{
				return SvnCommandType::Unknown;
			}
		}

		property bool BerkeleyDBKeepTransactionLogs
		{
			bool get()
			{
				return _bdbKeepLogs;
			}
			void set(bool value)
			{
				_bdbKeepLogs = value;
			}
		}

		property SvnRepositoryFileSystem RepositoryType
		{
			SvnRepositoryFileSystem get()
			{
				return _reposType;
			}
			void set(SvnRepositoryFileSystem value)
			{
				_reposType = EnumVerifier::Verify(value);
			}
		}

		property SvnRepositoryCompatibility RepositoryCompatibility
		{
			SvnRepositoryCompatibility get()
			{
				return _reposCompat;
			}
			void set(SvnRepositoryCompatibility value)
			{
				_reposCompat = EnumVerifier::Verify(value);
			}
		}

        /// <summary>If not <c>null</c> allows creates a repository with this UUID</summary>
        property Nullable<Guid> RepositoryUuid
        {
            Nullable<Guid> get()
            {
                return _uuid;
            }

            void set(Nullable<Guid> value)
            {
                _uuid = value;
            }
        }
	};
}