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
#include "Args/Update.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;


[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Update(System.Collections.Generic.ICollection`1<System.String>,SharpSvn.SvnUpdateArgs,SharpSvn.SvnUpdateResult&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Update(System.Collections.Generic.ICollection`1<System.String>,SharpSvn.SvnUpdateResult&):System.Boolean", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Update(System.String,SharpSvn.SvnUpdateArgs,SharpSvn.SvnUpdateResult&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Update(System.String,SharpSvn.SvnUpdateResult&):System.Boolean", MessageId="1#")];

bool SvnClient::Update(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	SvnUpdateResult^ result;
	return Update(NewSingleItemCollection(path), gcnew SvnUpdateArgs(), result);
}

bool SvnClient::Update(String^ path, [Out] SvnUpdateResult^% updateResult)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	return Update(NewSingleItemCollection(path), gcnew SvnUpdateArgs(), updateResult);
}


bool SvnClient::Update(String^ path, SvnUpdateArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnUpdateResult^ result;

	return Update(NewSingleItemCollection(path), args, result);
}

bool SvnClient::Update(String^ path, SvnUpdateArgs^ args, [Out] SvnUpdateResult^% updateResult)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	return Update(NewSingleItemCollection(path), args, updateResult);
}

bool SvnClient::Update(ICollection<String^>^ paths)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");

	SvnUpdateResult^ result;

	return Update(paths, gcnew SvnUpdateArgs(), result);
}

bool SvnClient::Update(ICollection<String^>^ paths, [Out] SvnUpdateResult^% updateResult)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");

	return Update(paths, gcnew SvnUpdateArgs(), updateResult);
}


bool SvnClient::Update(ICollection<String^>^ paths, SvnUpdateArgs^ args)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnUpdateResult^ result;

	return Update(paths, args, result);
}

bool SvnClient::Update(ICollection<String^>^ paths, SvnUpdateArgs^ args, [Out] SvnUpdateResult^% updateResult)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	updateResult = nullptr;

	for each (String^ s in paths)
	{
		if (String::IsNullOrEmpty(s))
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "paths");
		else if (!IsNotUri(s))
			throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "paths");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprArray<String^, AprCStrPathMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCStrPathMarshaller^>(paths, %pool);

	apr_array_header_t* revs = nullptr;
	svn_opt_revision_t uRev = args->Revision->Or(SvnRevision::Head)->ToSvnRevision();

	svn_error_t *r = svn_client_update3(&revs,
		aprPaths->Handle,
		&uRev,
		(svn_depth_t)args->Depth,
		args->KeepDepth,
		args->IgnoreExternals,
		args->AllowObstructions,
		CtxHandle,
		pool.Handle);

	if (args->HandleResult(this, r))
	{
		AprArray<__int64, AprSvnRevNumMarshaller^>^ aprRevs = gcnew AprArray<__int64, AprSvnRevNumMarshaller^>(revs, %pool);

		updateResult = gcnew SvnUpdateResult(paths, safe_cast<IList<__int64>^>(aprRevs->ToArray()), (paths->Count >= 1) ? aprRevs[0] : -1);

		return true;
	}

	return false;
}
