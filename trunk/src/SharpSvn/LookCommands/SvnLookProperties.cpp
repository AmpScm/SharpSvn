// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "LookArgs/SvnLookPropertyArgs.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnLookClient::Property(String^ repositoryPath, String^ propertyName, SvnLookPropertyArgs^ args)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	repositoryPath = SvnBase::PreparePath(repositoryPath);

	svn_error_t* r;
	svn_repos_t* repos;
	if (r = svn_repos_open(&repos, pool.AllocString(repositoryPath), pool.Handle))
		return args->HandleResult(this, r);

	svn_fs_t* fs = svn_repos_fs(repos);

	svn_string_t* val;
	
	if (!String::IsNullOrEmpty(args->Transaction))
	{
		svn_fs_txn_t* txn = NULL;
		if (r = svn_fs_open_txn(&(txn), fs, pool.AllocString(args->Transaction), pool.Handle))
			return args->HandleResult(this, r);
		if (r = svn_fs_txn_prop(&val, txn, pool.AllocString(propertyName), pool.Handle))
			return args->HandleResult(this, r);
	}
	else
	{
		svn_revnum_t rev;

		if (!SVN_IS_VALID_REVNUM(args->Revision->Revision) ||
			(args->Revision->Revision == 0))
		{
			if (r = svn_fs_youngest_rev(&rev, fs, pool.Handle))
				return args->HandleResult(this, r);
		}
		else
		{
			rev = (svn_revnum_t)args->Revision->Revision;
		}

		if (r = svn_fs_revision_prop(&val, fs, rev, pool.AllocString(propertyName), pool.Handle))
			return args->HandleResult(this, r);

		args->ResultRevision = rev;
	}

	if (val && val->data)
	{
		args->Result = SvnBase::Utf8_PtrToString(val->data, (int)(val->len));
	}

	return args->HandleResult(this, r);
}

bool SvnLookClient::GetProperty(String^ repositoryPath, String^ propertyName, SvnLookPropertyArgs^ args, [Out] String^% result)
{
	bool r = Property(repositoryPath, propertyName, args);
	if (r)
	{
		result = args->Result;
	}
	return r;
}

bool SvnLookClient::GetAuthor(String^ repositoryPath, SvnLookPropertyArgs^ args, [Out] String^% result)
{
	return GetProperty(repositoryPath, gcnew String(SVN_PROP_REVISION_AUTHOR), args, result);
}

bool SvnLookClient::GetLog(String^ repositoryPath, SvnLookPropertyArgs^ args, [Out] String^% result)
{
	return GetProperty(repositoryPath, gcnew String(SVN_PROP_REVISION_LOG), args, result);
}

bool SvnLookClient::GetDate(String^ repositoryPath, SvnLookPropertyArgs^ args, [Out] DateTime^% result)
{
	String^ str;
	bool r = GetProperty(repositoryPath, gcnew String(SVN_PROP_REVISION_DATE), args, str);
	if (r)
	{
		AprPool pool(%_pool);
		apr_time_t t;
		svn_error_t* err;
		if (err = svn_time_from_cstring(&t, pool.AllocString(str), pool.Handle))
			return args->HandleResult(this, err);
		result = SvnBase::DateTimeFromAprTime(t);
	}
	return r;
}
