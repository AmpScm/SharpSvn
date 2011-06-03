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

bool SvnClient::Copy(SvnTarget^ source, String^ toPath)
{
	if (!source)
		throw gcnew ArgumentNullException("source");
	else if (String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if (!IsNotUri(toPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	return Copy(NewSingleItemCollection(source), toPath, gcnew SvnCopyArgs());
}

generic<typename TSvnTarget> where TSvnTarget : SvnTarget
bool SvnClient::Copy(ICollection<TSvnTarget>^ sources, String^ toPath)
{
	if (!sources)
		throw gcnew ArgumentNullException("sources");
	else if (String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if (!IsNotUri(toPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	return Copy<TSvnTarget>(sources, toPath, gcnew SvnCopyArgs());
}

bool SvnClient::Copy(SvnTarget^ source, String^ toPath, SvnCopyArgs^ args)
{
	if (!source)
		throw gcnew ArgumentNullException("source");
	else if (String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(toPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	return Copy(NewSingleItemCollection(source), toPath, args);
}

generic<typename TSvnTarget> where TSvnTarget : SvnTarget
bool SvnClient::Copy(ICollection<TSvnTarget>^ sources, String^ toPath, SvnCopyArgs^ args)
{
	if (!sources)
		throw gcnew ArgumentNullException("sources");
	else if (String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(toPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");
	else if (!sources->Count)
		throw gcnew ArgumentException(SharpSvnStrings::CollectionMustContainAtLeastOneItem, "sources");

	bool isFirst = true;
	bool isLocal = false;

	for each (SvnTarget^ target in sources)
	{
		if (!target)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "sources");

		SvnPathTarget^ pt = dynamic_cast<SvnPathTarget^>(target);
		if (isFirst)
		{
			isLocal = (nullptr != pt);
			isFirst = false;
		}
		else if (isLocal != (nullptr != pt))
			throw gcnew ArgumentException(SharpSvnStrings::AllTargetsMustBeUriOrPath, "sources");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_commit_info_t* pInfo = nullptr;

	apr_array_header_t* copies = AllocCopyArray<TSvnTarget>(sources, %pool);

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
		args->AlwaysCopyAsChild || (sources->Count > 1),
		args->CreateParents,
		args->IgnoreExternals,
		nullptr,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r, sources);
}

bool SvnClient::RemoteCopy(SvnTarget^ source, Uri^ toUri)
{
	if (!source)
		throw gcnew ArgumentNullException("source");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if (!SvnBase::IsValidReposUri(toUri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "toUri");

	SvnCommitResult^ result = nullptr;

	return RemoteCopy(NewSingleItemCollection(source), toUri, gcnew SvnCopyArgs(), result);
}

generic<typename TSvnTarget> where TSvnTarget : SvnTarget
bool SvnClient::RemoteCopy(ICollection<TSvnTarget>^ sources, Uri^ toUri)
{
	if (!sources)
		throw gcnew ArgumentNullException("sources");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");

	SvnCommitResult^ result = nullptr;

	return RemoteCopy<TSvnTarget>(sources, toUri, gcnew SvnCopyArgs(), result);
}

bool SvnClient::RemoteCopy(SvnTarget^ source, Uri^ toUri, [Out] SvnCommitResult^% result)
{
	if (!source)
		throw gcnew ArgumentNullException("source");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");

	return RemoteCopy(NewSingleItemCollection(source), toUri, gcnew SvnCopyArgs(), result);
}

generic<typename TSvnTarget> where TSvnTarget : SvnTarget
bool SvnClient::RemoteCopy(ICollection<TSvnTarget>^ sources, Uri^ toUri, [Out] SvnCommitResult^% result)
{
	if (!sources)
		throw gcnew ArgumentNullException("sources");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");

	return RemoteCopy<TSvnTarget>(sources, toUri, gcnew SvnCopyArgs(), result);
}

bool SvnClient::RemoteCopy(SvnTarget^ source, Uri^ toUri, SvnCopyArgs^ args)
{
	if (!source)
		throw gcnew ArgumentNullException("source");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitResult^ result = nullptr;
	return RemoteCopy(NewSingleItemCollection(source), toUri, args, result);
}

generic<typename TSvnTarget> where TSvnTarget : SvnTarget
bool SvnClient::RemoteCopy(ICollection<TSvnTarget>^ sources, Uri^ toUri, SvnCopyArgs^ args)
{
	if (!sources)
		throw gcnew ArgumentNullException("sources");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitResult^ result = nullptr;
	return RemoteCopy<TSvnTarget>(sources, toUri, args, result);
}

bool SvnClient::RemoteCopy(SvnTarget^ source, Uri^ toUri, SvnCopyArgs^ args, [Out] SvnCommitResult^% result)
{
	if (!source)
		throw gcnew ArgumentNullException("source");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	return RemoteCopy(NewSingleItemCollection(source), toUri, args, result);
}

generic<typename TSvnTarget> where TSvnTarget : SvnTarget
bool SvnClient::RemoteCopy(ICollection<TSvnTarget>^ sources, Uri^ toUri, SvnCopyArgs^ args, [Out] SvnCommitResult^% result)
{
	if (!sources)
		throw gcnew ArgumentNullException("sources");
	else if (!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!SvnBase::IsValidReposUri(toUri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "toUri");
	else if (!sources->Count)
		throw gcnew ArgumentException(SharpSvnStrings::CollectionMustContainAtLeastOneItem, "sources");

	bool isFirst = true;
	bool isLocal = false;

	for each (SvnTarget^ target in sources)
	{
		if (!target)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "sources");

		SvnPathTarget^ pt = dynamic_cast<SvnPathTarget^>(target);
		if (isFirst)
		{
			isLocal = (nullptr != pt);
			isFirst = false;
		}
		else if (isLocal != (nullptr != pt))
			throw gcnew ArgumentException(SharpSvnStrings::AllTargetsMustBeUriOrPath, "sources");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	apr_array_header_t* copies = AllocCopyArray<TSvnTarget>(sources, %pool);

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
		args->AlwaysCopyAsChild || (sources->Count > 1),
		args->CreateParents,
		args->IgnoreExternals,
		CreateRevPropList(args->LogProperties, %pool),
		CtxHandle,
		pool.Handle);

	if (commitInfoPtr)
		result = SvnCommitResult::Create(this, args, commitInfoPtr, %pool);
	else
		result = nullptr;

	return args->HandleResult(this, r, sources);
}

