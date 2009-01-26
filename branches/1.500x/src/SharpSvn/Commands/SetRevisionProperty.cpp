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

bool SvnClient::SetRevisionProperty(SvnUriTarget^ target, String^ propertyName, String^ value)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!value)
		throw gcnew ArgumentNullException("value");

	return SetRevisionProperty(target, propertyName, gcnew SvnSetRevisionPropertyArgs(), value);
}

bool SvnClient::SetRevisionProperty(SvnUriTarget^ target, String^ propertyName, ICollection<Byte>^ bytes)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!bytes)
		throw gcnew ArgumentNullException("bytes");

	return SetRevisionProperty(target, propertyName, gcnew SvnSetRevisionPropertyArgs(), bytes);
}

bool SvnClient::SetRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnSetRevisionPropertyArgs^ args, String^ value)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!value)
		throw gcnew ArgumentNullException("value");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	AprPool pool(%_pool);

	return InternalSetRevisionProperty(target,
		propertyName,
		// Subversion does no normalization on the property value; so we have to do this before sending it
		// to the server
		pool.AllocPropertyValue(value, propertyName),
		args,
		%pool);
}

bool SvnClient::SetRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnSetRevisionPropertyArgs^ args, ICollection<Byte>^ bytes)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!bytes)
		throw gcnew ArgumentNullException("bytes");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	AprPool pool(%_pool);

	array<Byte> ^byteArray = dynamic_cast<array<Byte>^>(bytes);

	if (!byteArray)
	{
		byteArray = gcnew array<Byte>(bytes->Count);

		bytes->CopyTo(byteArray, 0);
	}

	return InternalSetRevisionProperty(target,
		propertyName,
		pool.AllocSvnString(byteArray),
		args,
		%pool);
}

bool SvnClient::DeleteRevisionProperty(SvnUriTarget^ target, String^ propertyName)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	return DeleteRevisionProperty(target, propertyName, gcnew SvnSetRevisionPropertyArgs());
}

bool SvnClient::DeleteRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnSetRevisionPropertyArgs^ args)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	AprPool pool(%_pool);

	return InternalSetRevisionProperty(target,
		propertyName,
		nullptr,
		args,
		%pool);
}

bool SvnClient::InternalSetRevisionProperty(SvnUriTarget^ target, String^ propertyName, const svn_string_t* value, SvnSetRevisionPropertyArgs^ args, AprPool^ pool)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized); // We might need repository access
	ArgsStore store(this, args);

	svn_revnum_t set_rev = 0;

	svn_error_t *r = svn_client_revprop_set(
		pool->AllocString(propertyName),
		value,
		pool->AllocString(target->SvnTargetName),
		target->Revision->AllocSvnRevision(pool),
		&set_rev,
		args->Force,
		CtxHandle,
		pool->Handle);

	return args->HandleResult(this, r);
}
