// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/GetProperty.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;


[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetProperty(SharpSvn.SvnTarget,System.String,SharpSvn.SvnGetPropertyArgs,SharpSvn.SvnTargetPropertyCollection&)", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetProperty(SharpSvn.SvnTarget,System.String,SharpSvn.SvnPropertyValue&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetProperty(SharpSvn.SvnTarget,System.String,System.String&)", MessageId="2#")];

bool SvnClient::GetProperty(SvnTarget^ target, String^ propertyName, String^% value)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!propertyName)
		throw gcnew ArgumentNullException("propertyName");

	SvnTargetPropertyCollection^ result = nullptr;
	value = nullptr;

	bool ok = GetProperty(target, propertyName, gcnew SvnGetPropertyArgs(), result);

	if (ok && result && (result->Count >= 0))
		value = static_cast<IList<SvnPropertyValue^>^>(result)[0]->StringValue;

	return ok;
}

bool SvnClient::GetProperty(SvnTarget^ target, String^ propertyName, SvnPropertyValue^% value)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!propertyName)
		throw gcnew ArgumentNullException("propertyName");

	SvnTargetPropertyCollection^ result;
	value = nullptr;

	if (GetProperty(target, propertyName, gcnew SvnGetPropertyArgs(), result))
	{
		if (result->Count)
			value = static_cast<IList<SvnPropertyValue^>^>(result)[0];

		return true;
	}
	else
		return false;
}

bool SvnClient::GetProperty(SvnTarget^ target, String^ propertyName, SvnGetPropertyArgs^ args, SvnTargetPropertyCollection^% properties)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!propertyName)
		throw gcnew ArgumentNullException("propertyName");

	properties = nullptr;
	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);
	
	svn_opt_revision_t pegRev = target->Revision->ToSvnRevision();
	svn_opt_revision_t rev = args->Revision->Or(target->Revision)->ToSvnRevision();
	svn_revnum_t actualRev = 0;

	apr_hash_t* pHash = nullptr;

	svn_error_t *r = svn_client_propget3(
		&pHash,
		pool.AllocString(propertyName),
		pool.AllocString(target->SvnTargetName),
		&pegRev,
		&rev,
		&actualRev,
		(svn_depth_t)args->Depth,
		CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
		CtxHandle,
		pool.Handle);

	if (pHash)
	{
		SvnTargetPropertyCollection^ rd = gcnew SvnTargetPropertyCollection();

		for (apr_hash_index_t* hi = apr_hash_first(pool.Handle, pHash); hi ; hi = apr_hash_next(hi))
		{
			const char* pKey;
			apr_ssize_t keyLen;
			const svn_string_t *propVal;

			apr_hash_this(hi, (const void**)&pKey, &keyLen, (void**)&propVal);

			Object^ val = PtrToStringOrByteArray(propVal->data, (int)propVal->len);

			String^ strVal = dynamic_cast<String^>(val);

			SvnTarget^ itemTarget = SvnTarget::FromString(Utf8_PtrToString(pKey, (int)keyLen));

			if (strVal)
				rd->Add(gcnew SvnPropertyValue(propertyName, strVal, itemTarget));
			else
				rd->Add(gcnew SvnPropertyValue(propertyName, safe_cast<array<Byte>^>(val), itemTarget));
		}

		properties = rd;
	}

	return args->HandleResult(this, r);
}

bool SvnClient::TryGetProperty(SvnTarget^ target, String^ propertyName, String^% value)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!propertyName)
		throw gcnew ArgumentNullException("propertyName");

	SvnTargetPropertyCollection^ result = nullptr;
	value = nullptr;

	SvnGetPropertyArgs^ args = gcnew SvnGetPropertyArgs();
	args->ThrowOnError = false;

	if (GetProperty(target, propertyName, args, result))
	{
		if (result->Count > 0)
		{
			value = static_cast<IList<SvnPropertyValue^>^>(result)[0]->StringValue;

			return true;
		}

		// Fall through if no property fetched
	}
	return false;
}