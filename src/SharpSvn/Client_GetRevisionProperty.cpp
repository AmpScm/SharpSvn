// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

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
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	return GetRevisionProperty(target, propertyName, gcnew SvnGetRevisionPropertyArgs(), value);
}

bool SvnClient::GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, [Out] SvnPropertyValue^% value)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	return GetRevisionProperty(target, propertyName, gcnew SvnGetRevisionPropertyArgs(), value);
}

bool SvnClient::GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnGetRevisionPropertyArgs^ args, [Out] String^% value)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!args)
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
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!args)
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
		pool.AllocString(target->TargetName),
		target->Revision->AllocSvnRevision(%pool),
		&set_rev,
		CtxHandle,
		pool.Handle);

	if (!r && result && result->data)
	{
		value = SvnPropertyValue::Create(pName, result, nullptr);
	}

	return args->HandleResult(this, r);
}