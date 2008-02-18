// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/Unlock.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;


[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.Unlock(System.String):System.Boolean")];
[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.Unlock(System.String,SharpSvn.SvnUnlockArgs):System.Boolean")];

bool SvnClient::Unlock(String^ target)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(!IsNotUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");

	return Unlock(NewSingleItemCollection(target), gcnew SvnUnlockArgs());
}

bool SvnClient::Unlock(Uri^ target)
{
	if (!target)
		throw gcnew ArgumentNullException("target");

	return Unlock(NewSingleItemCollection(target), gcnew SvnUnlockArgs());
}

bool SvnClient::Unlock(ICollection<String^>^ targets)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");

	return Unlock(targets, gcnew SvnUnlockArgs());
}

bool SvnClient::Unlock(ICollection<Uri^>^ targets)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");

	return Unlock(targets, gcnew SvnUnlockArgs());
}


bool SvnClient::Unlock(String^ target, SvnUnlockArgs^ args)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return Unlock(NewSingleItemCollection(target), args);
}

bool SvnClient::Unlock(Uri^ target, SvnUnlockArgs^ args)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return Unlock(NewSingleItemCollection(target), args);
}

bool SvnClient::Unlock(ICollection<String^>^ targets, SvnUnlockArgs^ args)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	for each (String^ target in targets)
	{
		if (String::IsNullOrEmpty(target))
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targets");
		else if(!IsNotUri(target))
			throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "targets");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprArray<String^, AprCStrPathMarshaller^>^ aprTargets = gcnew AprArray<String^, AprCStrPathMarshaller^>(targets, %pool);

	return UnlockInternal(aprTargets, args, %pool);
}

bool SvnClient::Unlock(ICollection<Uri^>^ targets, SvnUnlockArgs^ args)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	array<String^>^ targetStrings = gcnew array<String^>(targets->Count);

	int i = 0;
	for each (Uri^ uri in targets)
	{
		if (!uri)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targets");
		else if(!SvnBase::IsValidReposUri(uri))
			throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "targets");

		targetStrings[i++] = uri->ToString();
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprArray<String^, AprCStrMarshaller^>^ aprTargets = gcnew AprArray<String^, AprCStrMarshaller^>(safe_cast<ICollection<String^>^>(targetStrings), %pool);

	return UnlockInternal(aprTargets, args, %pool);
}

generic<typename TMarshaller> where TMarshaller : IItemMarshaller<String^>
bool SvnClient::UnlockInternal(AprArray<String^, TMarshaller>^ items, SvnUnlockArgs^ args, AprPool^ pool)
{
	if(!items)
		throw gcnew ArgumentNullException("items");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	svn_error_t* r = svn_client_unlock(
		items->Handle,
		args->BreakLock,
		CtxHandle,
		pool->Handle);

	return args->HandleResult(this, r);
}