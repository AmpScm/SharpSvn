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
#include "Args/Commit.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Commit(System.Collections.Generic.ICollection`1<System.String>,SharpSvn.SvnCommitArgs,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Commit(System.Collections.Generic.ICollection`1<System.String>,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Commit(System.String,SharpSvn.SvnCommitArgs,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Commit(System.String,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="1#")];

bool SvnClient::Commit(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	SvnCommitResult^ result = nullptr;

	return Commit(NewSingleItemCollection(path), gcnew SvnCommitArgs(), result);
}

bool SvnClient::Commit(String^ path, [Out] SvnCommitResult^% result)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	return Commit(NewSingleItemCollection(path), gcnew SvnCommitArgs(), result);
}

bool SvnClient::Commit(ICollection<String^>^ paths)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");

	SvnCommitResult^ result = nullptr;

	return Commit(paths, gcnew SvnCommitArgs(), result);
}

bool SvnClient::Commit(ICollection<String^>^ paths, [Out] SvnCommitResult^% result)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");

	return Commit(paths, gcnew SvnCommitArgs(), result);
}

bool SvnClient::Commit(String^ path, SvnCommitArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitResult^ result = nullptr;

	return Commit(NewSingleItemCollection(path), args, result);
}

bool SvnClient::Commit(String^ path, SvnCommitArgs^ args, [Out] SvnCommitResult^% result)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	return Commit(NewSingleItemCollection(path), args, result);
}

bool SvnClient::Commit(ICollection<String^>^ paths, SvnCommitArgs^ args)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitResult^ result = nullptr;

	return Commit(paths, args, result);
}

bool SvnClient::Commit(ICollection<String^>^ paths, SvnCommitArgs^ args, [Out] SvnCommitResult^% result)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	for each (String^ path in paths)
	{
		if (String::IsNullOrEmpty(path))
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "paths");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	AprArray<String^, AprCStrDirentMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCStrDirentMarshaller^>(paths, %pool);

	svn_commit_info_t *commitInfoPtr = nullptr;

	svn_error_t *r = svn_client_commit4(
		&commitInfoPtr,
		aprPaths->Handle,
		(svn_depth_t)args->Depth,
		args->KeepLocks,
		args->KeepChangeLists,
		CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
		CreateRevPropList(args->LogProperties, %pool),
		CtxHandle,
		pool.Handle);

	if (commitInfoPtr)
		result = SvnCommitResult::Create(this, args, commitInfoPtr, %pool);
	else
		result = nullptr;

	return args->HandleResult(this, r, paths);
}
