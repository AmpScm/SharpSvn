// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/GetEligableMergeInfo.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetEligableMergeInfo(SharpSvn.SvnTarget,System.Uri,SharpSvn.SvnGetEligableMergeInfoArgs,SharpSvn.SvnEligableMergeInfo&)", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetEligableMergeInfo(SharpSvn.SvnTarget,System.Uri,SharpSvn.SvnEligableMergeInfo&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.#GetEligableMergeInfo(SharpSvn.SvnTarget,System.Uri,SharpSvn.SvnGetEligableMergeInfoArgs,SharpSvn.SvnEligableMergeInfo&)")];

bool SvnClient::GetEligableMergeInfo(SvnTarget ^target, SvnUriTarget^ source, [Out]SvnEligableMergeInfo^% mergeInfo)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(!source)
		throw gcnew ArgumentNullException("source");
	
	return GetEligableMergeInfo(target, source, gcnew SvnGetEligableMergeInfoArgs(), mergeInfo);
}

bool SvnClient::GetEligableMergeInfo(SvnTarget ^target, SvnUriTarget^ source, SvnGetEligableMergeInfoArgs^ args, [Out]SvnEligableMergeInfo^% mergeInfo)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(!source)
		throw gcnew ArgumentNullException("source");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	apr_array_header_t* svnMergeInfo = nullptr;

	svn_error_t* r = svn_client_mergeinfo_get_eligible(
		&svnMergeInfo,
		pool.AllocString(target->SvnTargetName),
		target->Revision->AllocSvnRevision(%pool),
		pool.AllocString(source->SvnTargetName),
		source->Revision->AllocSvnRevision(%pool),
		CtxHandle,
		pool.Handle);

	if (!r && svnMergeInfo)
	{
		mergeInfo = gcnew SvnEligableMergeInfo(target, svnMergeInfo, %pool);
	}
	else
		mergeInfo = nullptr;

	return args->HandleResult(this, r) && mergeInfo;
}