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
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

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

	return args->HandleResult(this, r, target);
}