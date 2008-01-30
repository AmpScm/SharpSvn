// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

// Included from SvnClientArgs.h

namespace SharpSvn {

	ref class SvnRepositoryClient;

	/// <summary>Extended Parameter container of <see cref="SvnRepositoryClient" />'s CreateRepository method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnCreateRepositoryArgs : public SvnClientArgs
	{
		bool _bdbNoFSyncCommit;
		bool _bdbKeepLogs;
		SvnRepositoryFilesystem _reposType;
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

		property SvnRepositoryFilesystem RepositoryType
		{
			SvnRepositoryFilesystem get()
			{
				return _reposType;
			}
			void set(SvnRepositoryFilesystem value)
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

	/// <summary>Extended Parameter container of <see cref="SvnRepositoryClient" />'s DeleteRepository method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnDeleteRepositoryArgs : public SvnClientArgs
	{
	public:
		SvnDeleteRepositoryArgs()
		{
		}
	};

	/// <summary>Extended Parameter container of <see cref="SvnRepositoryClient" />'s RecoverRepository method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnRecoverRepositoryArgs : public SvnClientArgs
	{
		bool _nonBlocking;
	public:
		SvnRecoverRepositoryArgs()
		{
		}

		property bool NonBlocking
		{
			bool get()
			{
				return _nonBlocking;
			}
			void set(bool value)
			{
				_nonBlocking = value;
			}
		}
	};
};