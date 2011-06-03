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
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded, SvnExtendedState::MimeTypesLoaded);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	svn_error_t *r = svn_client_add4(
		pool.AllocPath(path),
		(svn_depth_t)args->Depth,
		args->Force,
		args->NoIgnore,
		args->AddParents,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r, path);
}
