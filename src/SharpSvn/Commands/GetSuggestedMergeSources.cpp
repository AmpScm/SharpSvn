// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

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