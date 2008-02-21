// $Id: Add.h 272 2008-02-14 17:46:34Z bhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "svn_repos.h"


namespace SharpSvn {

	public enum class SvnRepositoryIdType
	{
		Default = svn_repos_load_uuid_default,
		Ignore = svn_repos_load_uuid_ignore,
		Force = svn_repos_load_uuid_force
	};

	/// <summary>Extended Parameter container of <see cref="SvnRepositoryClient" />'s CreateRepository method</summary>
	/// <threadsafety static="true" instance="false"/>
	public ref class SvnLoadRepositoryArgs : public SvnClientArgs
	{
		SvnRepositoryIdType _loadIdType;
		String^ _parentDir;
		bool _noPreCommitHook;
		bool _noPostCommitHook;

	public:
		SvnLoadRepositoryArgs()
		{
		}

		virtual property SvnClientCommandType ClientCommandType
		{
			virtual SvnClientCommandType get() override sealed
			{
				return SvnClientCommandType::Unknown;
			}
		}

		property SvnRepositoryIdType LoadIdType
		{
			SvnRepositoryIdType get()
			{
				return _loadIdType;
			}
			void set(SvnRepositoryIdType value)
			{
				_loadIdType = EnumVerifier::Verify(value);
			}
		}

		property bool RunPreCommitHook
		{
			bool get()
			{
				return !_noPreCommitHook;
			}
			void set(bool value)
			{
				_noPreCommitHook = !value;
			}
		}

		property bool RunPostCommitHook
		{
			bool get()
			{
				return !_noPostCommitHook;
			}
			void set(bool value)
			{
				_noPostCommitHook = !value;
			}
		}

		property String^ ImportParent
		{
			String^ get()
			{
				return _parentDir;
			}
			void set(String^ value)
			{
				_parentDir = value;
			}
		}
	};
}