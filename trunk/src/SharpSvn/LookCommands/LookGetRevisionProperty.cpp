// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "LookArgs/SvnLookRevisionPropertyArgs.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnLookClient::GetRevisionProperty(String^ repositoryPath, String^ propertyName, [Out] String^% value)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");

	return GetRevisionProperty(repositoryPath, propertyName, gcnew SvnLookRevisionPropertyArgs(), value);
}

bool SvnLookClient::GetRevisionProperty(String^ repositoryPath, String^ propertyName, [Out] SvnPropertyValue^% value)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");

	return GetRevisionProperty(repositoryPath, propertyName, gcnew SvnLookRevisionPropertyArgs(), value);
}

bool SvnLookClient::GetRevisionProperty(String^ repositoryPath, String^ propertyName, SvnLookRevisionPropertyArgs^ args, [Out] String^% result)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");

	SvnPropertyValue^ value = nullptr;
	result = nullptr;

	try
	{
		return GetRevisionProperty(repositoryPath, propertyName, args, value);
	}
	finally
	{
		if (value)
			result = value->StringValue;
	}
}

bool SvnLookClient::GetRevisionProperty(String^ repositoryPath, String^ propertyName, SvnLookRevisionPropertyArgs^ args, [Out] SvnPropertyValue^% value)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	value = nullptr;

	svn_error_t* r;
	svn_repos_t* repos;
	if (r = svn_repos_open(&repos, pool.AllocCanonical(repositoryPath), pool.Handle))
		return args->HandleResult(this, r);

	svn_fs_t* fs = svn_repos_fs(repos);

	svn_string_t* val;

	const char* pName = pool.AllocString(propertyName);
	
	if (!String::IsNullOrEmpty(args->Transaction))
	{
		svn_fs_txn_t* txn = NULL;
		if (r = svn_fs_open_txn(&txn, fs, pool.AllocString(args->Transaction), pool.Handle))
			return args->HandleResult(this, r);

		if (r = svn_fs_txn_prop(&val, txn, pName, pool.Handle))
			return args->HandleResult(this, r);
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
		{
			rev = (svn_revnum_t)args->Revision;
		}

		if (r = svn_fs_revision_prop(&val, fs, rev, pName, pool.Handle))
			return args->HandleResult(this, r);
	}

	if(val)
		value = SvnPropertyValue::Create(pName, val, nullptr, propertyName);

	return args->HandleResult(this, r);
}


/*bool SvnLookClient::GetAuthor(String^ repositoryPath, SvnLookPropertyArgs^ args, [Out] String^% result)
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
}*/
