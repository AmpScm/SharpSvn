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
#include "Args/GetRevisionProperty.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.GetRevisionProperty(SharpSvn.SvnUriTarget,System.String,SharpSvn.SvnGetRevisionPropertyArgs,SharpSvn.SvnPropertyValue&):System.Boolean")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetRevisionProperty(SharpSvn.SvnUriTarget,System.String,SharpSvn.SvnGetRevisionPropertyArgs,SharpSvn.SvnPropertyValue&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetRevisionProperty(SharpSvn.SvnUriTarget,System.String,SharpSvn.SvnPropertyValue&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetRevisionProperty(SharpSvn.SvnUriTarget,System.String,SharpSvn.SvnGetRevisionPropertyArgs,System.String&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetRevisionProperty(SharpSvn.SvnUriTarget,System.String,System.String&):System.Boolean", MessageId="2#")];

bool SvnClient::GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, [Out] String^% value)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	return GetRevisionProperty(target, propertyName, gcnew SvnGetRevisionPropertyArgs(), value);
}

bool SvnClient::GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, [Out] SvnPropertyValue^% value)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	return GetRevisionProperty(target, propertyName, gcnew SvnGetRevisionPropertyArgs(), value);
}

bool SvnClient::GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnGetRevisionPropertyArgs^ args, [Out] String^% value)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnPropertyValue^ propertyValue = nullptr;
	value = nullptr;

	try
	{
		return GetRevisionProperty(target, propertyName, args, propertyValue);
	}
	finally
	{
		if (propertyValue)
			value = propertyValue->StringValue;
	}
}

bool SvnClient::GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnGetRevisionPropertyArgs^ args, [Out] SvnPropertyValue^% value)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_string_t *result = nullptr;
	svn_revnum_t set_rev = 0;

	const char* pName = pool.AllocString(propertyName);

	svn_error_t* r = svn_client_revprop_get(
		pName,
		&result,
		pool.AllocString(target->SvnTargetName),
		target->GetSvnRevision(SvnRevision::None, SvnRevision::Head)->AllocSvnRevision(%pool),
		&set_rev,
		CtxHandle,
		pool.Handle);

	if (!r && result && result->data)
	{
		value = SvnPropertyValue::Create(pName, result, nullptr);
	}

	return args->HandleResult(this, r);
}