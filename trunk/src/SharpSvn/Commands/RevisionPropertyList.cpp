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
#include "Args/RevisionPropertyList.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetRevisionPropertyList(System.Uri,SharpSvn.SvnRevision,SharpSvn.SvnPropertyCollection&)", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetRevisionPropertyList(System.Uri,SharpSvn.SvnRevision,SharpSvn.SvnRevisionPropertyListArgs,SharpSvn.SvnPropertyCollection&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.#GetRevisionPropertyList(System.Uri,SharpSvn.SvnRevision,SharpSvn.SvnRevisionPropertyListArgs,SharpSvn.SvnPropertyCollection&)")];

bool SvnClient::GetRevisionPropertyList(Uri^ target, SvnRevision^ revision, [Out] SvnPropertyCollection^% list)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (!SvnBase::IsValidReposUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "target");
	else if (!revision->IsExplicit)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");

	return GetRevisionPropertyList(target, revision, gcnew SvnRevisionPropertyListArgs(), list);
}

bool SvnClient::GetRevisionPropertyList(Uri^ target, SvnRevision^ revision, SvnRevisionPropertyListArgs^ args, [Out] SvnPropertyCollection^% list)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!SvnBase::IsValidReposUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "target");
	else if (!revision->IsExplicit)
		throw gcnew ArgumentException(SharpSvnStrings::TargetMustContainExplicitRevision, "revision");

	EnsureState(SvnContextState::AuthorizationInitialized);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	apr_hash_t *props = nullptr;
	svn_revnum_t set_rev = 0;

	svn_error_t* r = svn_client_revprop_list(
		&props,
		pool.AllocUri(target),
		revision->AllocSvnRevision(%pool),
		&set_rev,
		CtxHandle,
		pool.Handle);

	if (!r && props)
	{
		list = CreatePropertyDictionary(props, %pool);
	}

	return args->HandleResult(this, r, target);
}
