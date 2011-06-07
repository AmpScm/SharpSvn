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
#include "Args/Revert.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;


bool SvnClient::Revert(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	return Revert(path, gcnew SvnRevertArgs());
}

bool SvnClient::Revert(String^ path, SvnRevertArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	return Revert(NewSingleItemCollection(path), args);
}

bool SvnClient::Revert(ICollection<String^>^ paths)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");

	return Revert(paths, gcnew SvnRevertArgs());
}

bool SvnClient::Revert(ICollection<String^>^ paths, SvnRevertArgs^ args)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	for each (String^ path in paths)
	{
		if (String::IsNullOrEmpty(path))
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "paths");
		else if (!IsNotUri(path))
			throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "paths");
	}

	EnsureState(SvnContextState::ConfigLoaded);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	AprArray<String^, AprCStrDirentMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCStrDirentMarshaller^>(paths, %pool);

	svn_error_t *r = svn_client_revert2(
		aprPaths->Handle,
		(svn_depth_t)args->Depth,
		CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r, paths);
}
