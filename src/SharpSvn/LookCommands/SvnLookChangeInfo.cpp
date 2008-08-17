// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "LookArgs/SvnChangeInfoArgs.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnLookClient::ChangeInfo(String^ repositoryPath, EventHandler<SvnChangeInfoEventArgs^>^ changeInfoHandler)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!changeInfoHandler)
		throw gcnew ArgumentNullException("changeInfoHandler");

	return ChangeInfo(repositoryPath, gcnew SvnChangeInfoArgs(), changeInfoHandler);
}

bool SvnLookClient::ChangeInfo(String^ repositoryPath, SvnChangeInfoArgs^ args, EventHandler<SvnChangeInfoEventArgs^>^ changeInfoHandler)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if (changeInfoHandler)
		args->ChangeInfo += changeInfoHandler;

	try
	{
		svn_repos_t* repos = nullptr;
		svn_fs_t* fs = nullptr;
		svn_error_t* r;
		apr_hash_t* props = nullptr;
		svn_log_entry_t *entry = svn_log_entry_create(pool.Handle);

		if (r = svn_repos_open(
							&repos,
							pool.AllocCanonical(repositoryPath),
							pool.Handle))
		{
			return args->HandleResult(this, r);
		}

		fs = svn_repos_fs(repos);

		if (!String::IsNullOrEmpty(args->Transaction))
		{
			svn_fs_txn_t* txn = nullptr;

			if (r = svn_fs_open_txn(&(txn), fs, pool.AllocString(args->Transaction), pool.Handle))
				return args->HandleResult(this, r);

			if (r = svn_fs_txn_proplist(&props, txn, pool.Handle))
				return args->HandleResult(this, r);

			entry->revision = -1;

			if (args->RetrieveChangedPaths)
				throw gcnew NotImplementedException("BH: TODO");
		}
		else
		{
			svn_revnum_t rev;

			if (args->Revision < 0L)
			{
				if (r = svn_fs_youngest_rev(&rev, fs, pool.Handle))
					return args->HandleResult(this, r);
			}
			else
				rev = (svn_revnum_t)args->Revision;

			if (r = svn_fs_revision_proplist(&props, fs, rev, pool.Handle))
				return args->HandleResult(this, r);

			entry->revision = rev;

			if (args->RetrieveChangedPaths)
				throw gcnew NotImplementedException("BH: TODO");
		}

		entry->revprops = props;

		SvnChangeInfoEventArgs^ e = gcnew SvnChangeInfoEventArgs(entry, %pool);
		try
		{
			args->OnChangeInfo(e);	
		}
		finally
		{
			e->Detach(false);
		}

		return args->HandleResult(this, r);
	}
	finally
	{
		if (changeInfoHandler)
			args->ChangeInfo -= changeInfoHandler;
	}
}

bool SvnLookClient::GetChangeInfo(String^ repositoryPath, [Out] SvnChangeInfoEventArgs^% changeInfo)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");

	return GetChangeInfo(repositoryPath, gcnew SvnChangeInfoArgs(), changeInfo);
}

bool SvnLookClient::GetChangeInfo(String^ repositoryPath, SvnChangeInfoArgs^ args, [Out] SvnChangeInfoEventArgs^% changeInfo)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");

	SingleInfoItemReceiver<SvnChangeInfoEventArgs^>^ result = gcnew SingleInfoItemReceiver<SvnChangeInfoEventArgs^>();

	try
	{
		return ChangeInfo(repositoryPath, args, result->Handler);
	}
	finally
	{
		changeInfo = result->Value;
	}
}