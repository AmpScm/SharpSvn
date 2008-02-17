// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/Add.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;

bool SvnClient::Add(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	return Add(path, gcnew SvnAddArgs());
}

bool SvnClient::Add(String^ path, SvnDepth depth)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	SvnAddArgs ^args = gcnew SvnAddArgs();

	args->Depth = depth;

	return Add(path, args);
}

bool SvnClient::Add(String^ path, SvnAddArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded, SvnExtendedState::MimeTypesLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_error_t *r = svn_client_add4(
		pool.AllocPath(path),
		(svn_depth_t)args->Depth,
		args->Force,
		args->NoIgnore,
		args->AddParents,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}
