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
#include "Args/Move.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteMove(System.Collections.Generic.ICollection`1<System.Uri>,System.Uri,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteMove(System.Collections.Generic.ICollection`1<System.Uri>,System.Uri,SharpSvn.SvnMoveArgs,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteMove(System.Uri,System.Uri,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteMove(System.Uri,System.Uri,SharpSvn.SvnMoveArgs,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="3#")];

bool SvnClient::Move(String^ sourcePath, String^ toPath)
{
	if (String::IsNullOrEmpty(sourcePath))
		throw gcnew ArgumentNullException("sourcePath");
	else if (String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");

	return Move(NewSingleItemCollection(sourcePath), toPath, gcnew SvnMoveArgs());
}

bool SvnClient::Move(ICollection<String^>^ sourcePaths, String^ toPath)
{
	if (!sourcePaths)
		throw gcnew ArgumentNullException("sourcePaths");
	else if (String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");

	return Move(sourcePaths, toPath, gcnew SvnMoveArgs());
}

bool SvnClient::Move(String^ sourcePath, String^ toPath, SvnMoveArgs^ args)
{
	if (String::IsNullOrEmpty(sourcePath))
		throw gcnew ArgumentNullException("sourcePath");
	else if (String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	return Move(NewSingleItemCollection(sourcePath), toPath, args);
}

bool SvnClient::Move(ICollection<String^>^ sourcePaths, String^ toPath, SvnMoveArgs^ args)
{
	if (!sourcePaths)
		throw gcnew ArgumentNullException("sourcePaths");
	else if (String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	for each (String^ s in sourcePaths)
	{
		if (String::IsNullOrEmpty(s))
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "sourcePaths");
		else if (!IsNotUri(s))
			throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "sourcePaths");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_commit_info_t* pInfo = nullptr;

	svn_error_t *r = svn_client_move5(
		&pInfo,
		AllocPathArray(sourcePaths, %pool),
		pool.AllocPath(toPath),
		args->Force,
		args->AlwaysMoveAsChild || (sourcePaths->Count > 1),
		args->CreateParents,
		nullptr,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r, sourcePaths);
}


bool SvnClient::RemoteMove(Uri^ sourceUri, Uri^ toUri)
{
	if (!sourceUri)
		throw gcnew ArgumentNullException("sourceUri");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if (!SvnBase::IsValidReposUri(sourceUri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "sourceUri");
	else if (!SvnBase::IsValidReposUri(toUri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "toUri");

	SvnCommitResult^ result = nullptr;

	return RemoteMove(NewSingleItemCollection(sourceUri), toUri, gcnew SvnMoveArgs(), result);
}

bool SvnClient::RemoteMove(ICollection<Uri^>^ sourceUris, Uri^ toUri)
{
	if (!sourceUris)
		throw gcnew ArgumentNullException("sourceUris");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if (!SvnBase::IsValidReposUri(toUri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "toUri");

	SvnCommitResult^ result = nullptr;

	return RemoteMove(sourceUris, toUri, gcnew SvnMoveArgs(), result);
}

bool SvnClient::RemoteMove(Uri^ sourceUri, Uri^ toUri, [Out] SvnCommitResult^% result)
{
	if (!sourceUri)
		throw gcnew ArgumentNullException("sourceUri");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if (!SvnBase::IsValidReposUri(sourceUri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "sourceUri");
	else if (!SvnBase::IsValidReposUri(toUri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "toUri");

	return RemoteMove(NewSingleItemCollection(sourceUri), toUri, gcnew SvnMoveArgs(), result);
}

bool SvnClient::RemoteMove(ICollection<Uri^>^ sourceUris, Uri^ toUri, [Out] SvnCommitResult^% result)
{
	if (!sourceUris)
		throw gcnew ArgumentNullException("sourceUris");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");

	return RemoteMove(sourceUris, toUri, gcnew SvnMoveArgs(), result);
}

bool SvnClient::RemoteMove(Uri^ sourceUri, Uri^ toUri, SvnMoveArgs^ args)
{
	if (!sourceUri)
		throw gcnew ArgumentNullException("sourceUri");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitResult^ result = nullptr;
	return RemoteMove(NewSingleItemCollection(sourceUri), toUri, args, result);
}

bool SvnClient::RemoteMove(ICollection<Uri^>^ sourceUris, Uri^ toUri, SvnMoveArgs^ args)
{
	if (!sourceUris)
		throw gcnew ArgumentNullException("sourceUris");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitResult^ result = nullptr;
	return RemoteMove(sourceUris, toUri, args, result);
}

bool SvnClient::RemoteMove(Uri^ sourceUri, Uri^ toUri, SvnMoveArgs^ args, [Out] SvnCommitResult^% result)
{
	if (!sourceUri)
		throw gcnew ArgumentNullException("sourceUri");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	return RemoteMove(NewSingleItemCollection(sourceUri), toUri, args, result);
}

bool SvnClient::RemoteMove(ICollection<Uri^>^ sourceUris, Uri^ toUri, SvnMoveArgs^ args, [Out] SvnCommitResult^% result)
{
	if (!sourceUris)
		throw gcnew ArgumentNullException("sourceUris");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!SvnBase::IsValidReposUri(toUri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "toUri");

	System::Collections::Generic::List<String^>^ uris = gcnew System::Collections::Generic::List<String^>(sourceUris->Count);

	for each (Uri^ u in sourceUris)
	{
		if (!u)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "sourceUris");
		else if (!SvnBase::IsValidReposUri(u))
			throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "sourceUris");

		uris->Add(UriToCanonicalString(u));
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_commit_info_t* commitInfoPtr = nullptr;

	svn_error_t *r = svn_client_move5(
		&commitInfoPtr,
		AllocArray(uris, %pool),
		pool.AllocCanonical(toUri),
		args->Force,
		args->AlwaysMoveAsChild || (sourceUris->Count > 1),
		args->CreateParents,
		CreateRevPropList(args->LogProperties, %pool),
		CtxHandle,
		pool.Handle);

	if (commitInfoPtr)
		result = SvnCommitResult::Create(this, args, commitInfoPtr, %pool);
	else
		result = nullptr;

	return args->HandleResult(this, r, sourceUris);
}
