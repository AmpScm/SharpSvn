// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::SetProperty(String^ path, String^ propertyName, String^ value)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!value)
		throw gcnew ArgumentNullException("value");

	return SetProperty(path, propertyName, value, gcnew SvnSetPropertyArgs());
}

bool SvnClient::SetProperty(String^ path, String^ propertyName, ICollection<Byte>^ bytes)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!bytes)
		throw gcnew ArgumentNullException("bytes");

	return SetProperty(path, propertyName, bytes, gcnew SvnSetPropertyArgs());
}

bool SvnClient::SetProperty(String^ path, String^ propertyName, String^ value, SvnSetPropertyArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!value)
		throw gcnew ArgumentNullException("value");

	AprPool pool(%_pool);

	return InternalSetProperty(path, propertyName, pool.AllocSvnString(value), args, %pool);
}

bool SvnClient::SetProperty(String^ path, String^ propertyName, ICollection<Byte>^ bytes, SvnSetPropertyArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!bytes)
		throw gcnew ArgumentNullException("bytes");

	AprPool pool(%_pool);

	array<Byte> ^byteArray = dynamic_cast<array<Byte>^>(bytes);

	if (!byteArray)
	{
		byteArray = gcnew array<Byte>(bytes->Count);

		bytes->CopyTo(byteArray, 0);
	}

	return InternalSetProperty(path, propertyName, pool.AllocSvnString(byteArray), args, %pool);
}

bool SvnClient::DeleteProperty(String^ path, String^ propertyName)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	return DeleteProperty(path, propertyName, gcnew SvnSetPropertyArgs());
}

bool SvnClient::DeleteProperty(String^ path, String^ propertyName, SvnSetPropertyArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	AprPool pool(%_pool);

	return InternalSetProperty(path, propertyName, nullptr, args, %pool);
}

bool SvnClient::InternalSetProperty(String^ path, String^ propertyName, const svn_string_t* value, SvnSetPropertyArgs^ args, AprPool^ pool)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	EnsureState(SvnContextState::AuthorizationInitialized); // We might need repository access
	ArgsStore store(this, args);

	svn_commit_info_t* pInfo = nullptr;

	const char* pcPropertyName = pool->AllocString(propertyName);

	if (!svn_prop_name_is_valid(pcPropertyName))
		throw gcnew ArgumentException("Property name is not valid", "propertyName");

	svn_error_t *r = svn_client_propset3(
		&pInfo,
		pcPropertyName,
		value,
		pool->AllocPath(path),
		(svn_depth_t)args->Depth,
		args->SkipChecks,
		(svn_revnum_t)args->BaseRevision,
		CreateChangeListsList(args->ChangeLists, pool), // Intersect ChangeLists
		CtxHandle,
		pool->Handle);

	return args->HandleResult(this, r);

}