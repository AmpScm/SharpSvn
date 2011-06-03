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
#include "Args/GetAppliedMergeInfo.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetAppliedMergeInfo(SharpSvn.SvnTarget,SharpSvn.SvnAppliedMergeInfo&):System.Boolean", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetAppliedMergeInfo(SharpSvn.SvnTarget,SharpSvn.SvnGetAppliedMergeInfoArgs,SharpSvn.SvnAppliedMergeInfo&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.GetAppliedMergeInfo(SharpSvn.SvnTarget,SharpSvn.SvnGetAppliedMergeInfoArgs,SharpSvn.SvnAppliedMergeInfo&):System.Boolean")];

bool SvnClient::GetAppliedMergeInfo(SvnTarget ^target, [Out]SvnAppliedMergeInfo^% mergeState)
{
	if (!target)
		throw gcnew ArgumentNullException("target");

	return GetAppliedMergeInfo(target, gcnew SvnGetAppliedMergeInfoArgs(), mergeState);
}

bool SvnClient::GetAppliedMergeInfo(SvnTarget ^target, SvnGetAppliedMergeInfoArgs^ args, [Out]SvnAppliedMergeInfo^% mergeState)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_mergeinfo_t svnMergeInfo = nullptr;

	svn_error_t* r = svn_client_mergeinfo_get_merged(
		&svnMergeInfo,
		pool.AllocString(target->SvnTargetName),
		target->Revision->AllocSvnRevision(%pool),
		CtxHandle,
		pool.Handle);

	if (!r && svnMergeInfo)
	{
		mergeState = gcnew SvnAppliedMergeInfo(target, svnMergeInfo, %pool);
	}
	else
		mergeState = nullptr;

	return args->HandleResult(this, r, target) && mergeState;
}