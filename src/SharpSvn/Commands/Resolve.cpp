// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/Resolve.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::Resolved(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	return Resolve(path, SvnAccept::Merged, gcnew SvnResolveArgs());
}

bool SvnClient::Resolve(String^ path, SvnAccept choice)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");	

	return Resolve(path, choice, gcnew SvnResolveArgs());
}

bool SvnClient::Resolve(String^ path, SvnAccept choice, SvnResolveArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	if(SvnAccept::Postpone == choice)
		throw gcnew ArgumentOutOfRangeException("choice");
	
	EnumVerifier::Verify(choice);

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_error_t *r = svn_client_resolve(
		pool.AllocPath(path),
		(svn_depth_t)args->Depth,
		(svn_wc_conflict_choice_t)choice,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}
