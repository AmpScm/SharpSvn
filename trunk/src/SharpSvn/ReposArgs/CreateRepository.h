// $Id: Add.h 272 2008-02-14 17:46:34Z bhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

	/// <summary>Extended Parameter container of <see cref="SvnRepositoryClient" />'s CreateRepository method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnCreateRepositoryArgs : public SvnClientArgs
	{
		bool _bdbNoFSyncCommit;
		bool _bdbKeepLogs;
		SvnRepositoryFileSystem _reposType;
		SvnRepositoryCompatibility _reposCompat;

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

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Unknown;
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
	};
}