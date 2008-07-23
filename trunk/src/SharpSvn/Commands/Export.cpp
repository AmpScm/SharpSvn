// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
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
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	result = nullptr;

	svn_revnum_t resultRev = 0;
	svn_opt_revision_t pegRev = from->Revision->ToSvnRevision();
	svn_opt_revision_t rev = args->Revision->Or(from->Revision)->Or(SvnRevision::Head)->ToSvnRevision();

	svn_error_t* r = svn_client_export4(
		&resultRev,
		pool.AllocString(from->SvnTargetName),
		pool.AllocPath(toPath),
		&pegRev,
		&rev,
		args->Overwrite,
		args->IgnoreExternals,
		(svn_depth_t)args->Depth,
		GetEolPtr(args->LineStyle),
		CtxHandle,
		pool.Handle);

	if (args->HandleResult(this, r))
	{
		result = SvnUpdateResult::Create(this, args, resultRev);
		return true;
	}
	else
		return false;
}
