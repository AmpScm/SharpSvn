// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/SetProperty.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::SetProperty(String^ target, String^ propertyName, String^ value)
{
	if (String::IsNullOrEmpty(target))
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!value)
		throw gcnew ArgumentNullException("value");
	else if(!IsNotUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");

	return SetProperty(target, propertyName, value, gcnew SvnSetPropertyArgs());
}

bool SvnClient::SetProperty(Uri^ target, String^ propertyName, String^ value)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!value)
		throw gcnew ArgumentNullException("value");

	return SetProperty(target, propertyName, value, gcnew SvnSetPropertyArgs());
}

bool SvnClient::SetProperty(String^ target, String^ propertyName, ICollection<Byte>^ bytes)
{
	if (String::IsNullOrEmpty(target))
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!IsNotUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");
	else if(!bytes)
		throw gcnew ArgumentNullException("bytes");

	return SetProperty(target, propertyName, bytes, gcnew SvnSetPropertyArgs());
}

bool SvnClient::SetProperty(Uri^ target, String^ propertyName, ICollection<Byte>^ bytes)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!bytes)
		throw gcnew ArgumentNullException("bytes");

	return SetProperty(target, propertyName, bytes, gcnew SvnSetPropertyArgs());
}

bool SvnClient::SetProperty(String^ target, String^ propertyName, String^ value, SvnSetPropertyArgs^ args)
{
	if (String::IsNullOrEmpty(target))
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!IsNotUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!value)
		throw gcnew ArgumentNullException("value");

	AprPool pool(%_pool);

	return InternalSetProperty(gcnew SvnPathTarget(target), propertyName, pool.AllocSvnString(value), args, %pool);
}

bool SvnClient::SetProperty(Uri^ target, String^ propertyName, String^ value, SvnSetPropertyArgs^ args)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!value)
		throw gcnew ArgumentNullException("value");

	AprPool pool(%_pool);

	return InternalSetProperty(gcnew SvnUriTarget(target), propertyName, pool.AllocSvnString(value), args, %pool);
}

bool SvnClient::SetProperty(String^ target, String^ propertyName, ICollection<Byte>^ bytes, SvnSetPropertyArgs^ args)
{
	if (String::IsNullOrEmpty(target))
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!IsNotUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");
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

	return InternalSetProperty(gcnew SvnPathTarget(target), propertyName, pool.AllocSvnString(byteArray), args, %pool);
}

bool SvnClient::SetProperty(Uri^ target, String^ propertyName, ICollection<Byte>^ bytes, SvnSetPropertyArgs^ args)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
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

	return InternalSetProperty(gcnew SvnUriTarget(target), propertyName, pool.AllocSvnString(byteArray), args, %pool);
}

bool SvnClient::DeleteProperty(String^ target, String^ propertyName)
{
	if (String::IsNullOrEmpty(target))
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!IsNotUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");

	return DeleteProperty(target, propertyName, gcnew SvnSetPropertyArgs());
}

bool SvnClient::DeleteProperty(Uri^ target, String^ propertyName)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	return DeleteProperty(target, propertyName, gcnew SvnSetPropertyArgs());
}

bool SvnClient::DeleteProperty(String^ target, String^ propertyName, SvnSetPropertyArgs^ args)
{
	if (String::IsNullOrEmpty(target))
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!IsNotUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");

	AprPool pool(%_pool);

	return InternalSetProperty(gcnew SvnPathTarget(target), propertyName, nullptr, args, %pool);
}

bool SvnClient::DeleteProperty(Uri^ target, String^ propertyName, SvnSetPropertyArgs^ args)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	AprPool pool(%_pool);

	return InternalSetProperty(gcnew SvnUriTarget(target), propertyName, nullptr, args, %pool);
}

bool SvnClient::InternalSetProperty(SvnTarget^ target, String^ propertyName, const svn_string_t* value, SvnSetPropertyArgs^ args, AprPool^ pool)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (target->Revision->RevisionType != SvnRevisionType::None)
		throw gcnew ArgumentException();

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
		pool->AllocString(target->SvnTargetName),
		(svn_depth_t)args->Depth,
		args->SkipChecks,
		(svn_revnum_t)args->BaseRevision,
		CreateChangeListsList(args->ChangeLists, pool), // Intersect ChangeLists
		CtxHandle,
		pool->Handle);

	return args->HandleResult(this, r);
}