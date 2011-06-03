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
#include "Args/Relocate.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::Relocate(String^ path, Uri^ from, Uri^ to)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!from)
		throw gcnew ArgumentNullException("from");
	else if (!to)
		throw gcnew ArgumentNullException("to");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if (!IsValidReposUri(from))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "from");
	else if (!IsValidReposUri(to))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "to");

	return Relocate(path, from, to, gcnew SvnRelocateArgs());
}

bool SvnClient::Relocate(String^ path, Uri^ from, Uri^ to, SvnRelocateArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!from)
		throw gcnew ArgumentNullException("from");
	else if (!to)
		throw gcnew ArgumentNullException("to");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if (!IsValidReposUri(from))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "from");
	else if (!IsValidReposUri(to))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "to");

	EnsureState(SvnContextState::AuthorizationInitialized);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	svn_error_t *r = svn_client_relocate(
		pool.AllocPath(path),
		pool.AllocCanonical(from),
		pool.AllocCanonical(to),
		!args->NonRecursive,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r, path);
}
