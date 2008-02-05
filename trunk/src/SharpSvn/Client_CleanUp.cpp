// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;
[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.CleanUp(System.String,SharpSvn.SvnCleanUpArgs):System.Boolean")];


bool SvnClient::CleanUp(String ^path)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	return CleanUp(path, gcnew SvnCleanUpArgs());
}

bool SvnClient::CleanUp(String ^path, SvnCleanUpArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_error_t *r = svn_client_cleanup(
		pool.AllocPath(path),
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}
