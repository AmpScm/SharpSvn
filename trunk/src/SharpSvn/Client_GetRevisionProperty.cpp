#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, [Out] String^% value)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	GetRevisionProperty(target, propertyName, gcnew SvnGetRevisionPropertyArgs(), value);
}

void SvnClient::GetRevisionProperty(SvnUriTarget^ target, String^ propertyName, [Out] IList<char>^% bytes)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	GetRevisionProperty(target, propertyName, gcnew SvnGetRevisionPropertyArgs(), bytes);
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

	return args->HandleResult(r);
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

	return args->HandleResult(r);
}