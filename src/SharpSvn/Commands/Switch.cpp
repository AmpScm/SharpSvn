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
#include "SvnUpdateResult.h"
#include "Args/Switch.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Switch(System.String,SharpSvn.SvnUriTarget,SharpSvn.SvnSwitchArgs,SharpSvn.SvnUpdateResult&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Switch(System.String,SharpSvn.SvnUriTarget,SharpSvn.SvnUpdateResult&):System.Boolean", MessageId="2#")];

bool SvnClient::Switch(String^ path, SvnUriTarget^ target)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if (!target)
		throw gcnew ArgumentNullException("target");

	SvnUpdateResult^ result;

	return Switch(path, target, gcnew SvnSwitchArgs(), result);
}

bool SvnClient::Switch(String^ path, SvnUriTarget^ target, [Out] SvnUpdateResult^% result)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if (!target)
		throw gcnew ArgumentNullException("target");

	return Switch(path, target, gcnew SvnSwitchArgs(), result);
}

bool SvnClient::Switch(String^ path, SvnUriTarget^ target, SvnSwitchArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnUpdateResult^ result;

	return Switch(path, target, args, result);
}

bool SvnClient::Switch(String^ path, SvnUriTarget^ target, SvnSwitchArgs^ args, [Out] SvnUpdateResult^% result)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if (!target)
		throw gcnew ArgumentNullException("args");

	if (args->Revision->RequiresWorkingCopy)
		throw gcnew ArgumentException(SharpSvnStrings::RevisionTypeMustBeHeadDateOrSpecific , "args");
	else if (target->Revision->RequiresWorkingCopy)
		throw gcnew ArgumentException(SharpSvnStrings::RevisionTypeMustBeHeadDateOrSpecific , "target");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_revnum_t rev = 0;
	svn_opt_revision_t pegRev = target->Revision->ToSvnRevision();
	svn_opt_revision_t toRev = args->Revision->Or(target->Revision)->Or(SvnRevision::Head)->ToSvnRevision();

	svn_error_t *r = svn_client_switch2(
		&rev,
		pool.AllocPath(path),
		pool.AllocString(target->SvnTargetName),
		&pegRev,
		&toRev,
		(svn_depth_t)args->Depth,
		args->KeepDepth,
		args->IgnoreExternals,
		args->AllowObstructions,
		CtxHandle,
		pool.Handle);

	result = SvnUpdateResult::Create(this, args, rev);

	return args->HandleResult(this, r);
}
