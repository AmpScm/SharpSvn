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
#include "Args/Copy.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#RemoteCopy`1(System.Collections.Generic.ICollection`1<!!0>,System.Uri,SharpSvn.SvnCopyArgs,SharpSvn.SvnCommitResult&)", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#RemoteCopy(SharpSvn.SvnTarget,System.Uri,SharpSvn.SvnCommitResult&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#RemoteCopy`1(System.Collections.Generic.ICollection`1<!!0>,System.Uri,SharpSvn.SvnCommitResult&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#RemoteCopy(SharpSvn.SvnTarget,System.Uri,SharpSvn.SvnCopyArgs,SharpSvn.SvnCommitResult&)", MessageId="3#")];

bool SvnClient::Copy(SvnTarget^ sourceTarget, String^ toPath)
{
	if (!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if (String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if (!IsNotUri(toPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	return Copy(NewSingleItemCollection(sourceTarget), toPath, gcnew SvnCopyArgs());
}

generic<typename TSvnTarget> where TSvnTarget : SvnTarget
bool SvnClient::Copy(ICollection<TSvnTarget>^ sourceTargets, String^ toPath)
{
	if (!sourceTargets)
		throw gcnew ArgumentNullException("sourceTargets");
	else if (String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if (!IsNotUri(toPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	return Copy<TSvnTarget>(sourceTargets, toPath, gcnew SvnCopyArgs());
}

bool SvnClient::Copy(SvnTarget^ sourceTarget, String^ toPath, SvnCopyArgs^ args)
{
	if (!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if (String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(toPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	return Copy(NewSingleItemCollection(sourceTarget), toPath, args);
}

generic<typename TSvnTarget> where TSvnTarget : SvnTarget
bool SvnClient::Copy(ICollection<TSvnTarget>^ sourceTargets, String^ toPath, SvnCopyArgs^ args)
{
	if (!sourceTargets)
		throw gcnew ArgumentNullException("sourceTargets");
	else if (String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(toPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");
	else if (!sourceTargets->Count)
		throw gcnew ArgumentException(SharpSvnStrings::CollectionMustContainAtLeastOneItem, "sourceTargets");

	bool isFirst = true;
	bool isLocal = false;

	for each (SvnTarget^ target in sourceTargets)
	{
		if (!target)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "sourceTargets");
		
		if (isFirst)
		{
			isLocal = (nullptr != dynamic_cast<SvnPathTarget^>(target));
			isFirst = false;
		}
		else if (isLocal != (nullptr != dynamic_cast<SvnPathTarget^>(target)))
			throw gcnew ArgumentException(SharpSvnStrings::AllTargetsMustBeUriOrPath, "sourceTargets");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_commit_info_t* pInfo = nullptr;

	apr_array_header_t* copies = AllocCopyArray<TSvnTarget>(sourceTargets, %pool);

	if(copies && args->Revision->RevisionType != SvnRevisionType::None)
	{
		svn_opt_revision_t* r = args->Revision->AllocSvnRevision(%pool);

		for(int i = 0; i < copies->nelts; i++)
		{
			svn_client_copy_source_t* cp = ((svn_client_copy_source_t**)copies->elts)[i];

			cp->revision = r;
		}
	}

	svn_error_t *r = svn_client_copy5(
		&pInfo,
		copies,
		pool.AllocPath(toPath),
		args->AlwaysCopyAsChild || (sourceTargets->Count > 1),
		args->CreateParents,
		args->IgnoreExternals,
		nullptr,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}

bool SvnClient::RemoteCopy(SvnTarget^ sourceTarget, Uri^ toUri)
{
	if (!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if (!SvnBase::IsValidReposUri(toUri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "toUri");

	SvnCommitResult^ result = nullptr;

	return RemoteCopy(NewSingleItemCollection(sourceTarget), toUri, gcnew SvnCopyArgs(), result);
}

generic<typename TSvnTarget> where TSvnTarget : SvnTarget
bool SvnClient::RemoteCopy(ICollection<TSvnTarget>^ sourceTargets, Uri^ toUri)
{
	if (!sourceTargets)
		throw gcnew ArgumentNullException("sourceTargets");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");

	SvnCommitResult^ result = nullptr;

	return RemoteCopy<TSvnTarget>(sourceTargets, toUri, gcnew SvnCopyArgs(), result);
}

bool SvnClient::RemoteCopy(SvnTarget^ sourceTarget, Uri^ toUri, [Out] SvnCommitResult^% result)
{
	if (!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");

	return RemoteCopy(NewSingleItemCollection(sourceTarget), toUri, gcnew SvnCopyArgs(), result);
}

generic<typename TSvnTarget> where TSvnTarget : SvnTarget
bool SvnClient::RemoteCopy(ICollection<TSvnTarget>^ sourceTargets, Uri^ toUri, [Out] SvnCommitResult^% result)
{
	if (!sourceTargets)
		throw gcnew ArgumentNullException("sourceTargets");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");

	return RemoteCopy<TSvnTarget>(sourceTargets, toUri, gcnew SvnCopyArgs(), result);
}

bool SvnClient::RemoteCopy(SvnTarget^ sourceTarget, Uri^ toUri, SvnCopyArgs^ args)
{
	if (!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitResult^ result = nullptr;
	return RemoteCopy(NewSingleItemCollection(sourceTarget), toUri, args, result);
}

generic<typename TSvnTarget> where TSvnTarget : SvnTarget
bool SvnClient::RemoteCopy(ICollection<TSvnTarget>^ sourceTargets, Uri^ toUri, SvnCopyArgs^ args)
{
	if (!sourceTargets)
		throw gcnew ArgumentNullException("sourceTargets");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitResult^ result = nullptr;
	return RemoteCopy<TSvnTarget>(sourceTargets, toUri, args, result);
}

bool SvnClient::RemoteCopy(SvnTarget^ sourceTarget, Uri^ toUri, SvnCopyArgs^ args, [Out] SvnCommitResult^% result)
{
	if (!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	return RemoteCopy(NewSingleItemCollection(sourceTarget), toUri, args, result);
}

generic<typename TSvnTarget> where TSvnTarget : SvnTarget
bool SvnClient::RemoteCopy(ICollection<TSvnTarget>^ sourceTargets, Uri^ toUri, SvnCopyArgs^ args, [Out] SvnCommitResult^% result)
{
	if (!sourceTargets)
		throw gcnew ArgumentNullException("sourceTargets");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!SvnBase::IsValidReposUri(toUri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "toUri");
	else if (!sourceTargets->Count)
		throw gcnew ArgumentException(SharpSvnStrings::CollectionMustContainAtLeastOneItem, "sourceTargets");

	bool isFirst = true;
	bool isLocal = false;

	for each (SvnTarget^ target in sourceTargets)
	{
		if (!target)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "sourceTargets");
		
		if (isFirst)
		{
			isLocal = (nullptr != dynamic_cast<SvnPathTarget^>(target));
			isFirst = false;
		}
		else if (isLocal != (nullptr != dynamic_cast<SvnPathTarget^>(target)))
			throw gcnew ArgumentException(SharpSvnStrings::AllTargetsMustBeUriOrPath, "sourceTargets");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	apr_array_header_t* copies = AllocCopyArray<TSvnTarget>(sourceTargets, %pool);

	if(copies && args->Revision->RevisionType != SvnRevisionType::None)
	{
		svn_opt_revision_t* r = args->Revision->AllocSvnRevision(%pool);

		for(int i = 0; i < copies->nelts; i++)
		{
			svn_client_copy_source_t* cp = ((svn_client_copy_source_t**)copies->elts)[i];

			cp->revision = r;
		}
	}

	svn_commit_info_t* commitInfoPtr = nullptr;

	svn_error_t *r = svn_client_copy5(
		&commitInfoPtr,
		copies,
		pool.AllocCanonical(toUri),
		args->AlwaysCopyAsChild || (sourceTargets->Count > 1),
		args->CreateParents,
		args->IgnoreExternals,
		CreateRevPropList(args->LogProperties, %pool),
		CtxHandle,
		pool.Handle);

	if (commitInfoPtr)
		result = SvnCommitResult::Create(this, args, commitInfoPtr, %pool);
	else
		result = nullptr;

	return args->HandleResult(this, r);
}

