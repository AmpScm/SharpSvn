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
#include "Args/GetSuggestedMergeSources.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetSuggestedMergeSources(SharpSvn.SvnTarget,SharpSvn.SvnGetSuggestedMergeSourcesArgs,SharpSvn.SvnMergeSourcesCollection&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetSuggestedMergeSources(SharpSvn.SvnTarget,SharpSvn.SvnMergeSourcesCollection&):System.Boolean", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.#GetSuggestedMergeSources(SharpSvn.SvnTarget,SharpSvn.SvnGetSuggestedMergeSourcesArgs,SharpSvn.SvnMergeSourcesCollection&)")];

bool SvnClient::GetSuggestedMergeSources(SvnTarget ^target, [Out]SvnMergeSourcesCollection^% mergeSources)
{
	if (!target)
		throw gcnew ArgumentNullException("target");

	return GetSuggestedMergeSources(target, gcnew SvnGetSuggestedMergeSourcesArgs(), mergeSources);
}

bool SvnClient::GetSuggestedMergeSources(SvnTarget ^target, SvnGetSuggestedMergeSourcesArgs^ args, [Out]SvnMergeSourcesCollection^% mergeSources)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);
	mergeSources = nullptr;

	apr_array_header_t* svnMergeSources = nullptr;

	svn_error_t* r = svn_client_suggest_merge_sources(
		&svnMergeSources,
		pool.AllocString(target->SvnTargetName),
		target->GetSvnRevision(SvnRevision::Working, SvnRevision::Head)->AllocSvnRevision(%pool),
		CtxHandle,
		pool.Handle);

	if (!r && svnMergeSources)
	{
		SvnMergeSourcesCollection^ sourceList = gcnew SvnMergeSourcesCollection();

		const char** sources = (const char**)svnMergeSources->elts;
		for (int i = 0; i < svnMergeSources->nelts; i++)
		{
			sourceList->Add(gcnew SvnMergeSource(Utf8_PtrToUri(sources[i], SvnNodeKind::Unknown)));
		}

		mergeSources = sourceList;
	}

	return args->HandleResult(this, r);
}