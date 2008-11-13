// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "LookArgs/SvnLookRevisionPropertyArgs.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnLookClient::GetRevisionProperty(SvnLookOrigin^ lookOrigin, String^ propertyName, [Out] String^% value)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	return GetRevisionProperty(lookOrigin, propertyName, gcnew SvnLookRevisionPropertyArgs(), value);
}

bool SvnLookClient::GetRevisionProperty(SvnLookOrigin^ lookOrigin, String^ propertyName, [Out] SvnPropertyValue^% value)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	return GetRevisionProperty(lookOrigin, propertyName, gcnew SvnLookRevisionPropertyArgs(), value);
}

bool SvnLookClient::GetRevisionProperty(SvnLookOrigin^ lookOrigin, String^ propertyName, SvnLookRevisionPropertyArgs^ args, [Out] String^% result)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnPropertyValue^ value = nullptr;
	result = nullptr;

	try
	{
		return GetRevisionProperty(lookOrigin, propertyName, args, value);
	}
	finally
	{
		if (value)
			result = value->StringValue;
	}
}

bool SvnLookClient::GetRevisionProperty(SvnLookOrigin^ lookOrigin, String^ propertyName, SvnLookRevisionPropertyArgs^ args, [Out] SvnPropertyValue^% value)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	value = nullptr;

	svn_error_t* r;
	svn_repos_t* repos;
	if (r = svn_repos_open(&repos, pool.AllocCanonical(lookOrigin->RepositoryPath), pool.Handle))
		return args->HandleResult(this, r);

	svn_fs_t* fs = svn_repos_fs(repos);

	svn_string_t* val;

	const char* pName = pool.AllocString(propertyName);
	
	if (lookOrigin->HasTransaction)
	{
		svn_fs_txn_t* txn = NULL;
		if (r = svn_fs_open_txn(&txn, fs, pool.AllocString(lookOrigin->Transaction), pool.Handle))
			return args->HandleResult(this, r);

		if (r = svn_fs_txn_prop(&val, txn, pName, pool.Handle))
			return args->HandleResult(this, r);
	}
	else
	{
		svn_revnum_t rev;

		if (!lookOrigin->HasRevision)
		{
			if (r = svn_fs_youngest_rev(&rev, fs, pool.Handle))
				return args->HandleResult(this, r);
		}
		else
		{
			rev = (svn_revnum_t)lookOrigin->Revision;
		}

		if (r = svn_fs_revision_prop(&val, fs, rev, pName, pool.Handle))
			return args->HandleResult(this, r);
	}

	if(val)
		value = SvnPropertyValue::Create(pName, val, nullptr, propertyName);

	return args->HandleResult(this, r);
}

