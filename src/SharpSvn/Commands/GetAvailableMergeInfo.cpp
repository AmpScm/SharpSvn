// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/GetAvailableMergeInfo.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetAvailableMergeInfo(SharpSvn.SvnTarget,System.Uri,SharpSvn.SvnAvailableMergeInfo&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetAvailableMergeInfo(SharpSvn.SvnTarget,System.Uri,SharpSvn.SvnGetSuggestedMergeSourcesArgs,SharpSvn.SvnAvailableMergeInfo&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.GetAvailableMergeInfo(SharpSvn.SvnTarget,System.Uri,SharpSvn.SvnGetSuggestedMergeSourcesArgs,SharpSvn.SvnAvailableMergeInfo&):System.Boolean")];

bool SvnClient::GetAvailableMergeInfo(SvnTarget ^target, Uri^ sourceUri, [Out]SvnAvailableMergeInfo^% mergeInfo)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(!sourceUri)
		throw gcnew ArgumentNullException("sourceUri");
	else if(!SvnBase::IsValidReposUri(sourceUri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "sourceUri");

	return GetAvailableMergeInfo(target, sourceUri, gcnew SvnGetAvailableMergeInfoArgs(), mergeInfo);
}

bool SvnClient::GetAvailableMergeInfo(SvnTarget ^target, Uri^ sourceUri, SvnGetAvailableMergeInfoArgs^ args, [Out]SvnAvailableMergeInfo^% mergeInfo)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(!sourceUri)
		throw gcnew ArgumentNullException("sourceUri");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!SvnBase::IsValidReposUri(sourceUri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "sourceUri");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	apr_array_header_t* svnMergeInfo = nullptr;

	svn_error_t* r = svn_client_mergeinfo_get_available(
		&svnMergeInfo,
		pool.AllocString(target->TargetName),
		target->Revision->AllocSvnRevision(%pool),
		pool.AllocCanonical(sourceUri->ToString()),
		CtxHandle,
		pool.Handle);

	if (!r && svnMergeInfo)
	{
		mergeInfo = gcnew SvnAvailableMergeInfo(target, svnMergeInfo, %pool);
	}
	else
		mergeInfo = nullptr;

	return args->HandleResult(this, r) && mergeInfo;
}