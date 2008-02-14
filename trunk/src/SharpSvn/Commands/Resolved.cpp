// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/Resolved.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::Resolved(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	return Resolved(path, gcnew SvnResolvedArgs());
}

bool SvnClient::Resolved(String^ path, SvnConflictChoice choice)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if(SvnConflictChoice::Postpone == choice)
		throw gcnew ArgumentOutOfRangeException("choice");

	return Resolved(path, gcnew SvnResolvedArgs(choice));
}

bool SvnClient::Resolved(String^ path, SvnResolvedArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_error_t *r = svn_client_resolved2(
		pool.AllocPath(path),
		(svn_depth_t)args->Depth,
		(svn_wc_conflict_choice_t)args->Choice,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}
