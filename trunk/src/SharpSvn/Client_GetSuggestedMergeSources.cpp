// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetSuggestedMergeSources(SharpSvn.SvnTarget,SharpSvn.SvnGetSuggestedMergeSourcesArgs,System.Collections.Generic.IList`1<System.Uri>&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetSuggestedMergeSources(SharpSvn.SvnTarget,System.Collections.Generic.IList`1<System.Uri>&):System.Boolean", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.GetSuggestedMergeSources(SharpSvn.SvnTarget,SharpSvn.SvnGetSuggestedMergeSourcesArgs,System.Collections.Generic.IList`1<System.Uri>&):System.Boolean")];

bool SvnClient::GetSuggestedMergeSources(SvnTarget ^target, [Out]IList<Uri^>^% mergeSources)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	return GetSuggestedMergeSources(target, gcnew SvnGetSuggestedMergeSourcesArgs(), mergeSources);
}

bool SvnClient::GetSuggestedMergeSources(SvnTarget ^target, SvnGetSuggestedMergeSourcesArgs^ args, [Out]IList<Uri^>^% mergeSources)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);
	mergeSources = nullptr;

	apr_array_header_t* svnMergeSources = nullptr;

	svn_opt_revision_t rev = target->GetSvnRevision(SvnRevision::Working, SvnRevision::Head);
	svn_error_t* r = svn_client_suggest_merge_sources(
		&svnMergeSources,
		pool.AllocString(target->TargetName),
		&rev,
		CtxHandle,
		pool.Handle);

	if(!r && svnMergeSources)
	{
		array<Uri^>^ uris = gcnew array<Uri^>(svnMergeSources->nelts);

		const char** sources = (const char**)svnMergeSources->elts;
		for(int i = 0; i < svnMergeSources->nelts; i++)
		{
			uris[i] = gcnew Uri(Utf8_PtrToString(sources[i]), UriKind::Absolute);
		}
		
		mergeSources = safe_cast<IList<Uri^>^>(uris);
	}

	return args->HandleResult(this, r);
}