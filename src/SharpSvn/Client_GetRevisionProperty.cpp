#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.GetRevisionProperty(SharpSvn.SvnUriTarget,System.String,SharpSvn.SvnGetRevisionPropertyArgs,System.Collections.Generic.IList`1<optional(System.Runtime.CompilerServices.IsSignUnspecifiedByte) System.SByte>&):System.Boolean")];
[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.GetRevisionPropertyList(SharpSvn.SvnUriTarget,SharpSvn.SvnRevisionPropertyListArgs,System.Collections.Generic.IDictionary`2<System.String,System.Object>&):System.Boolean")];
[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.GetRevisionProperty(SharpSvn.SvnUriTarget,System.String,SharpSvn.SvnGetRevisionPropertyArgs,System.String&):System.Boolean")];

bool SvnClient::GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, [Out] String^% value)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	return GetRevisionProperty(target, propertyName, gcnew SvnGetRevisionPropertyArgs(), value);
}

bool SvnClient::GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, [Out] IList<char>^% bytes)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	return GetRevisionProperty(target, propertyName, gcnew SvnGetRevisionPropertyArgs(), bytes);
}

bool SvnClient::GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnGetRevisionPropertyArgs^ args, [Out] String^% value)
{
	if(!target)
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

	svn_error_t* r = svn_client_revprop_get(
		pool.AllocString(propertyName),
		&result,
		pool.AllocString(target->TargetName),
		target->Revision->AllocSvnRevision(%pool),
		&set_rev,
		CtxHandle,
		pool.Handle);

	if(!r || !result || !result->data)
	{
		value = Utf8_PtrToString(result->data, (int)result->len);

		if(propertyName->StartsWith("svn:")) // Make sure properties return in the same format they were added
			value = value->Replace("\n", Environment::NewLine);
	}
	else
		value = nullptr;

	return args->HandleResult(this, r);
}

bool SvnClient::GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, SvnGetRevisionPropertyArgs^ args, [Out] IList<char>^% value)
{
	if(!target)
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

	svn_error_t* r = svn_client_revprop_get(
		pool.AllocString(propertyName),
		&result,
		pool.AllocString(target->TargetName),
		target->Revision->AllocSvnRevision(%pool),
		&set_rev,
		CtxHandle,
		pool.Handle);

	if(!r || !result || !result->data)
	{
		value = safe_cast<IList<char>^>(PtrToByteArray(result->data, (int)result->len));
	}
	else
		value = nullptr;

	return args->HandleResult(this, r);
}