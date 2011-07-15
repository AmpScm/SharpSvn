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


bool SvnClient::Lock(String^ target, SvnLockArgs^ args)
{
	if (String::IsNullOrEmpty(target))
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	return Lock(NewSingleItemCollection(target), args);
}

bool SvnClient::Lock(ICollection<String^>^ targets, SvnLockArgs^ args)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!targets->Count)
		throw gcnew ArgumentException(SharpSvnStrings::CollectionMustContainAtLeastOneItem, "targets");

	for each (String^ target in targets)
	{
		if (String::IsNullOrEmpty(target))
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targets");
		else if (!IsNotUri(target))
			throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "targets");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	AprArray<String^, AprCStrDirentMarshaller^>^ aprTargets = gcnew AprArray<String^, AprCStrDirentMarshaller^>(targets, %pool);

	return LockInternal(aprTargets, args, %pool);
}

bool SvnClient::RemoteLock(Uri^ target, String^ comment)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!comment)
		throw gcnew ArgumentNullException("comment");
	else if (!IsValidReposUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "target");

	SvnLockArgs^ args = gcnew SvnLockArgs();
	args->Comment = comment;

	return RemoteLock(NewSingleItemCollection(target), args);
}

bool SvnClient::RemoteLock(ICollection<Uri^>^ targets, String^ comment)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");
	else if (!comment)
		throw gcnew ArgumentNullException("comment");

	SvnLockArgs^ args = gcnew SvnLockArgs();
	args->Comment = comment;

	return RemoteLock(targets, args);
}

bool SvnClient::RemoteLock(Uri^ target, SvnLockArgs^ args)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsValidReposUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "target");

	return RemoteLock(NewSingleItemCollection(target), args);
}


bool SvnClient::RemoteLock(ICollection<Uri^>^ targets, SvnLockArgs^ args)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!targets->Count)
		throw gcnew ArgumentException(SharpSvnStrings::CollectionMustContainAtLeastOneItem, "targets");

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
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	AprArray<String^, AprUriMarshaller^>^ aprTargets = gcnew AprArray<String^, AprUriMarshaller^>(safe_cast<ICollection<String^>^>(targetStrings), %pool);

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

	args->Reset();
	svn_error_t* r = svn_client_lock(
		items->Handle,
		pool->AllocString(args->Comment),
		args->StealLock,
		CtxHandle,
		pool->Handle);

	if (r || !args->LockResult)
		return args->HandleResult(this, r, nullptr);
	else
		return args->HandleResult(this, args->LockResult, nullptr);
}