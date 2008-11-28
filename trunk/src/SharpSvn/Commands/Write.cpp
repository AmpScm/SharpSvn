// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "SvnStreamWrapper.h"
#include "Args/Write.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;


bool SvnClient::Write(SvnTarget^ target, Stream^ output)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!output)
		throw gcnew ArgumentNullException("output");

	return Write(target, output, gcnew SvnWriteArgs());
}

bool SvnClient::Write(SvnTarget^ target, Stream^ output, SvnWriteArgs^ args)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!output)
		throw gcnew ArgumentNullException("output");
	else if (!args)
		throw gcnew ObjectDisposedException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	SvnStreamWrapper wrapper(output, false, true, %pool);

	svn_opt_revision_t pegRev = target->Revision->ToSvnRevision();
	svn_opt_revision_t rev = args->Revision->Or(target->Revision)->ToSvnRevision();

	svn_error_t *r = svn_client_cat2(
		wrapper.Handle,
		pool.AllocString(target->SvnTargetName),
		&pegRev,
		&rev,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}