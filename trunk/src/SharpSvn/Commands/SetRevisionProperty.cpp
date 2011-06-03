// $Id$
//
// Copyright 2007-2009 The SharpSvn Project
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

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/SetRevisionProperty.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::SetRevisionProperty(Uri^ target, SvnRevision^ revision, String^ propertyName, String^ value)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!value)
		throw gcnew ArgumentNullException("value");
	else if (!SvnBase::IsValidReposUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "target");
	else if (!revision->IsExplicit)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");

	return SetRevisionProperty(target, revision, propertyName, value, gcnew SvnSetRevisionPropertyArgs());
}

bool SvnClient::SetRevisionProperty(Uri^ target, SvnRevision^ revision, String^ propertyName, ICollection<Byte>^ bytes)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!bytes)
		throw gcnew ArgumentNullException("bytes");
	else if (!SvnBase::IsValidReposUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "target");
	else if (!revision->IsExplicit)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");

	return SetRevisionProperty(target, revision, propertyName, bytes, gcnew SvnSetRevisionPropertyArgs());
}

bool SvnClient::SetRevisionProperty(Uri^ target, SvnRevision^ revision, String^ propertyName, String^ value, SvnSetRevisionPropertyArgs^ args)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!value)
		throw gcnew ArgumentNullException("value");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!SvnBase::IsValidReposUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "target");
	else if (!revision->IsExplicit)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");

	AprPool pool(%_pool);

	return InternalSetRevisionProperty(
		target,
		revision,
		propertyName,
		// Subversion does no normalization on the property value; so we have to do this before sending it
		// to the server
		pool.AllocPropertyValue(value, propertyName),
		nullptr,
		args,
		%pool);
}

bool SvnClient::SetRevisionProperty(Uri^ target, SvnRevision^ revision, String^ propertyName, ICollection<Byte>^ bytes, SvnSetRevisionPropertyArgs^ args)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!bytes)
		throw gcnew ArgumentNullException("bytes");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!SvnBase::IsValidReposUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "target");
	else if (!revision->IsExplicit)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");

	AprPool pool(%_pool);

	array<Byte> ^byteArray = dynamic_cast<array<Byte>^>(bytes);

	if (!byteArray)
	{
		byteArray = gcnew array<Byte>(bytes->Count);

		bytes->CopyTo(byteArray, 0);
	}

	return InternalSetRevisionProperty(
		target,
		revision,
		propertyName,
		pool.AllocSvnString(byteArray),
		nullptr,
		args,
		%pool);
}

bool SvnClient::DeleteRevisionProperty(Uri^ target, SvnRevision^ revision, String^ propertyName)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!SvnBase::IsValidReposUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "target");
	else if (!revision->IsExplicit)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");

	return DeleteRevisionProperty(target, revision, propertyName, gcnew SvnSetRevisionPropertyArgs());
}

bool SvnClient::DeleteRevisionProperty(Uri^ target, SvnRevision^ revision, String^ propertyName, SvnSetRevisionPropertyArgs^ args)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!SvnBase::IsValidReposUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "target");
	else if (!revision->IsExplicit)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");

	AprPool pool(%_pool);

	return InternalSetRevisionProperty(
		target,
		revision,
		propertyName,
		nullptr,
		nullptr,
		args,
		%pool);
}

bool SvnClient::InternalSetRevisionProperty(Uri^ target, SvnRevision^ revision, String^ propertyName, const svn_string_t* value, const svn_string_t* original_value, SvnSetRevisionPropertyArgs^ args, AprPool^ pool)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized); // We might need repository access
	ArgsStore store(this, args, pool);

	const char* pName = pool->AllocString(propertyName);

	if (!svn_prop_name_is_valid(pName))
		throw gcnew ArgumentException(SharpSvnStrings::PropertyNameIsNotValid, "propertyName");

	svn_revnum_t set_rev = 0;

	svn_error_t *r = svn_client_revprop_set2(
		pName,
		value,
		original_value,
		pool->AllocCanonical(target),
		revision->AllocSvnRevision(pool),
		&set_rev,
		args->Force,
		CtxHandle,
		pool->Handle);

	return args->HandleResult(this, r, target);
}
