// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/Lock.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.Lock(System.String,SharpSvn.SvnLockArgs):System.Boolean")];
[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.Lock(System.String,System.String):System.Boolean")];

bool SvnClient::Lock(String^ target, String^ comment)
{
	if (String::IsNullOrEmpty(target))
		throw gcnew ArgumentNullException("target");
	else if (!comment)
		throw gcnew ArgumentNullException("comment");

	SvnLockArgs^ args = gcnew SvnLockArgs();
	args->Comment = comment;

	return Lock(NewSingleItemCollection(target), args);
}

bool SvnClient::Lock(Uri^ target, String^ comment)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!comment)
		throw gcnew ArgumentNullException("comment");
	else if (!IsValidReposUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "target");

	SvnLockArgs^ args = gcnew SvnLockArgs();
	args->Comment = comment;

	return Lock(NewSingleItemCollection(target), args);
}

bool SvnClient::Lock(ICollection<String^>^ targets, String^ comment)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");
	else if (!comment)
		throw gcnew ArgumentNullException("comment");

	SvnLockArgs^ args = gcnew SvnLockArgs();
	args->Comment = comment;

	return Lock(targets, args);
}

bool SvnClient::Lock(ICollection<Uri^>^ targets, String^ comment)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");
	else if (!comment)
		throw gcnew ArgumentNullException("comment");

	SvnLockArgs^ args = gcnew SvnLockArgs();
	args->Comment = comment;

	return Lock(targets, args);
}

bool SvnClient::Lock(String^ target, SvnLockArgs^ args)
{
	if (String::IsNullOrEmpty(target))
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	return Lock(NewSingleItemCollection(target), args);
}

bool SvnClient::Lock(Uri^ target, SvnLockArgs^ args)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsValidReposUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "target");

	return Lock(NewSingleItemCollection(target), args);
}

bool SvnClient::Lock(ICollection<String^>^ targets, SvnLockArgs^ args)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	for each (String^ target in targets)
	{
		if (String::IsNullOrEmpty(target))
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targets");
		else if (!IsNotUri(target))
			throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "targets");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprArray<String^, AprCStrPathMarshaller^>^ aprTargets = gcnew AprArray<String^, AprCStrPathMarshaller^>(targets, %pool);

	return LockInternal(aprTargets, args, %pool);
}

bool SvnClient::Lock(ICollection<Uri^>^ targets, SvnLockArgs^ args)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	array<String^>^ targetStrings = gcnew array<String^>(targets->Count);

	int i = 0;
	for each (Uri^ uri in targets)
	{
		if (!uri)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targets");
		else if (!SvnBase::IsValidReposUri(uri))
			throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "targets");

		targetStrings[i++] = UriToCanonicalString(uri);
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprArray<String^, AprCanonicalMarshaller^>^ aprTargets = gcnew AprArray<String^, AprCanonicalMarshaller^>(safe_cast<ICollection<String^>^>(targetStrings), %pool);

	return LockInternal(aprTargets, args, %pool);
}

generic<typename TMarshaller> where TMarshaller : IItemMarshaller<String^>
bool SvnClient::LockInternal(AprArray<String^, TMarshaller>^ items, SvnLockArgs^ args, AprPool^ pool)
{
	if (!items)
		throw gcnew ArgumentNullException("items");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!pool)
		throw gcnew ArgumentNullException("pool");

	svn_error_t* r = svn_client_lock(
		items->Handle,
		pool->AllocString(args->Comment),
		args->StealLock,
		CtxHandle,
		pool->Handle);

	return args->HandleResult(this, r);
}