// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/Relocate.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::Relocate(String^ path, Uri^ from, Uri^ to)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!to)
		throw gcnew ArgumentNullException("to");
	else if(!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if(!IsValidReposUri(from))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "from");
	else if(!IsValidReposUri(to))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "to");

	return Relocate(path, from, to, gcnew SvnRelocateArgs());
}

bool SvnClient::Relocate(String^ path, Uri^ from, Uri^ to, SvnRelocateArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!to)
		throw gcnew ArgumentNullException("to");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if(!IsValidReposUri(from))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "from");
	else if(!IsValidReposUri(to))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "to");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_error_t *r = svn_client_relocate(
		pool.AllocPath(path),
		pool.AllocCanonical(from),
		pool.AllocCanonical(to),
		!args->NonRecursive,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}
