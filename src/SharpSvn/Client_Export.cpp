// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

const char* SvnClient::GetEolPtr(SvnLineStyle style)
{
	switch(style)
	{
	case SvnLineStyle::Native:
		return nullptr;
	case SvnLineStyle::CariageReturnLineFeed:
		return "CRLF";
	case SvnLineStyle::LineFeed:
		return "LF";
	case SvnLineStyle::CariageReturn:
		return "CR";
	default:
		throw gcnew ArgumentOutOfRangeException("style");
	}
}


bool SvnClient::Export(SvnTarget^ from, String^ toPath)
{
	if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!toPath)
		throw gcnew ArgumentNullException("toPath");

	__int64 revision;

	return Export(from, toPath, gcnew SvnExportArgs(), revision);
}

bool SvnClient::Export(SvnTarget^ from, String^ toPath, [Out] __int64% revision)
{
	if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!toPath)
		throw gcnew ArgumentNullException("toPath");

	return Export(from, toPath, gcnew SvnExportArgs(), revision);
}

bool SvnClient::Export(SvnTarget^ from, String^ toPath, SvnExportArgs^ args)
{
	if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!toPath)
		throw gcnew ArgumentNullException("toPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	__int64 revision;
	return Export(from, toPath, args, revision);
}

bool SvnClient::Export(SvnTarget^ from, String^ toPath, SvnExportArgs^ args, [Out] __int64% revision)
{
	if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!toPath)
		throw gcnew ArgumentNullException("toPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	revision = -1;

	svn_revnum_t resultRev = 0;
	svn_opt_revision_t pegRev = from->Revision->ToSvnRevision();
	svn_opt_revision_t rev = args->Revision->ToSvnRevision(SvnRevision::Head);

	svn_error_t* r = svn_client_export4(
		&resultRev,
		pool.AllocString(from->TargetName),
		pool.AllocPath(toPath),
		&pegRev,
		&rev,
		args->Overwrite,
		args->IgnoreExternals,
		(svn_depth_t)args->Depth,
		GetEolPtr(args->LineStyle),
		CtxHandle,
		pool.Handle);

	if(args->HandleResult(this, r))
	{
		revision = resultRev;
		return true;
	}
	else
		return false;
}
