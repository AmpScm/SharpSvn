// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/CheckOut.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.CheckOut(SharpSvn.SvnUriTarget,System.String,SharpSvn.SvnCheckOutArgs,SharpSvn.SvnUpdateResult&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.CheckOut(SharpSvn.SvnUriTarget,System.String,SharpSvn.SvnUpdateResult&):System.Boolean", MessageId="2#")];

bool SvnClient::CheckOut(SvnUriTarget^ url, String^ path)
{
	if (!url)
		throw gcnew ArgumentNullException("url");
	else if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	SvnUpdateResult^ result;

	return CheckOut(url, path, gcnew SvnCheckOutArgs(), result);
}

bool SvnClient::CheckOut(SvnUriTarget^ url, String^ path, [Out] SvnUpdateResult^% result)
{
	if (!url)
		throw gcnew ArgumentNullException("url");
	else if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	return CheckOut(url, path, gcnew SvnCheckOutArgs(), result);
}

bool SvnClient::CheckOut(SvnUriTarget^ url, String^ path, SvnCheckOutArgs^ args)
{
	if (!url)
		throw gcnew ArgumentNullException("url");
	else if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnUpdateResult^ result;

	return CheckOut(url, path, args, result);
}

bool SvnClient::CheckOut(SvnUriTarget^ url, String^ path, SvnCheckOutArgs^ args, [Out] SvnUpdateResult^% result)
{
	if (!url)
		throw gcnew ArgumentNullException("url");
	else if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	switch(args->Revision->RevisionType)
	{
		case SvnRevisionType::None: // Translated to head
		case SvnRevisionType::Time:
		case SvnRevisionType::Number:
		case SvnRevisionType::Head:
			break;
		default:
			// Throw the error before we allocate the unmanaged resources
			throw gcnew ArgumentException(SharpSvnStrings::RevisionTypeMustBeHeadDateOrSpecific , "args");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_revnum_t version = 0;

	//svn_opt_peg_Re
	svn_opt_revision_t pegRev = url->Revision->ToSvnRevision();
	svn_opt_revision_t coRev = args->Revision->ToSvnRevision(SvnRevision::Head);

	svn_error_t* r = svn_client_checkout3(&version,
		pool.AllocString(url->SvnTargetName),
		pool.AllocPath(path),
		&pegRev,
		&coRev,
		(svn_depth_t)args->Depth,
		args->IgnoreExternals,
		args->AllowObstructions,
		CtxHandle,
		pool.Handle);

	result = SvnUpdateResult::Create(this, args, version);

	return args->HandleResult(this, r);
}
