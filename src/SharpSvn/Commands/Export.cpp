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
#include "Args/Export.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Export(SharpSvn.SvnTarget,System.String,SharpSvn.SvnExportArgs,SharpSvn.SvnUpdateResult&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Export(SharpSvn.SvnTarget,System.String,SharpSvn.SvnUpdateResult&):System.Boolean", MessageId="2#")];

bool SvnClient::Export(SvnTarget^ from, String^ toPath)
{
	if (!from)
		throw gcnew ArgumentNullException("from");
	else if (!toPath)
		throw gcnew ArgumentNullException("toPath");
	else if (!IsNotUri(toPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	SvnUpdateResult^ result;

	return Export(from, toPath, gcnew SvnExportArgs(), result);
}

bool SvnClient::Export(SvnTarget^ from, String^ toPath, [Out] SvnUpdateResult^% result)
{
	if (!from)
		throw gcnew ArgumentNullException("from");
	else if (!toPath)
		throw gcnew ArgumentNullException("toPath");
	else if (!IsNotUri(toPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	return Export(from, toPath, gcnew SvnExportArgs(), result);
}

bool SvnClient::Export(SvnTarget^ from, String^ toPath, SvnExportArgs^ args)
{
	if (!from)
		throw gcnew ArgumentNullException("from");
	else if (!toPath)
		throw gcnew ArgumentNullException("toPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnUpdateResult^ result;

	return Export(from, toPath, args, result);
}

bool SvnClient::Export(SvnTarget^ from, String^ toPath, SvnExportArgs^ args, [Out] SvnUpdateResult^% result)
{
	if (!from)
		throw gcnew ArgumentNullException("from");
	else if (!toPath)
		throw gcnew ArgumentNullException("toPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	result = nullptr;

	svn_revnum_t resultRev = 0;
	svn_opt_revision_t pegRev = from->Revision->ToSvnRevision();
	svn_opt_revision_t rev = args->Revision->Or(from->GetSvnRevision(SvnRevision::Working, SvnRevision::Head))->ToSvnRevision();

	svn_error_t* r = svn_client_export5(
		&resultRev,
		from->AllocAsString(%pool),
		pool.AllocDirent(toPath),
		&pegRev,
		&rev,
		args->Overwrite,
		args->IgnoreExternals,
		args->IgnoreKeywords,
		(svn_depth_t)args->Depth,
		GetEolPtr(args->LineStyle),
		CtxHandle,
		pool.Handle);

	if (args->HandleResult(this, r, from))
	{
		result = SvnUpdateResult::Create(this, args, resultRev);
		return true;
	}
	else
		return false;
}
