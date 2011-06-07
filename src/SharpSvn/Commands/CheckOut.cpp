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
#include "Args/CheckOut.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.CheckOut(SharpSvn.SvnUriTarget,System.String,SharpSvn.SvnCheckOutArgs,SharpSvn.SvnUpdateResult&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.CheckOut(SharpSvn.SvnUriTarget,System.String,SharpSvn.SvnUpdateResult&):System.Boolean", MessageId="2#")];

bool SvnClient::CheckOut(SvnUriTarget^ url, String^ path)
{
	if (!url)
		throw gcnew ArgumentNullException("url");
	else if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	SvnUpdateResult^ result;

	return CheckOut(url, path, gcnew SvnCheckOutArgs(), result);
}

bool SvnClient::CheckOut(SvnUriTarget^ url, String^ path, [Out] SvnUpdateResult^% result)
{
	if (!url)
		throw gcnew ArgumentNullException("url");
	else if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	return CheckOut(url, path, gcnew SvnCheckOutArgs(), result);
}

bool SvnClient::CheckOut(SvnUriTarget^ url, String^ path, SvnCheckOutArgs^ args)
{
	if (!url)
		throw gcnew ArgumentNullException("url");
	else if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnUpdateResult^ result;

	return CheckOut(url, path, args, result);
}

bool SvnClient::CheckOut(SvnUriTarget^ url, String^ path, SvnCheckOutArgs^ args, [Out] SvnUpdateResult^% result)
{
	if (!url)
		throw gcnew ArgumentNullException("url");
	else if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	if (args->Revision->RequiresWorkingCopy)
		throw gcnew ArgumentException(SharpSvnStrings::RevisionTypeMustBeHeadDateOrSpecific , "args");
	else if (url->Revision->RequiresWorkingCopy)
		throw gcnew ArgumentException(SharpSvnStrings::RevisionTypeMustBeHeadDateOrSpecific , "url");

	EnsureState(SvnContextState::AuthorizationInitialized);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	svn_revnum_t version = 0;

	svn_opt_revision_t pegRev = url->Revision->ToSvnRevision();
	svn_opt_revision_t coRev = args->Revision->Or(url->Revision)->Or(SvnRevision::Head)->ToSvnRevision();

	svn_error_t* r = svn_client_checkout3(&version,
		pool.AllocString(url->SvnTargetName),
		pool.AllocDirent(path),
		&pegRev,
		&coRev,
		(svn_depth_t)args->Depth,
		args->IgnoreExternals,
		args->AllowObstructions,
		CtxHandle,
		pool.Handle);

	result = SvnUpdateResult::Create(this, args, version);

	return args->HandleResult(this, r, url);
}
