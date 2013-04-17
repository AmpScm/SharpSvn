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
#include "Args/CreateDirectory.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteCreateDirectories(System.Collections.Generic.ICollection`1<System.Uri>,SharpSvn.SvnCreateDirectoryArgs,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="2#")]
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteCreateDirectory(System.Uri,SharpSvn.SvnCreateDirectoryArgs,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="2#")];

bool SvnClient::CreateDirectory(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	return CreateDirectories(NewSingleItemCollection(path), gcnew SvnCreateDirectoryArgs());
}

bool SvnClient::CreateDirectory(String^ path, SvnCreateDirectoryArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	return CreateDirectories(NewSingleItemCollection(path), args);
}


bool SvnClient::CreateDirectories(ICollection<String^>^ paths)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");

	return CreateDirectories(paths, gcnew SvnCreateDirectoryArgs());
}

bool SvnClient::CreateDirectories(ICollection<String^>^ paths, SvnCreateDirectoryArgs^ args)
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

	svn_error_t *r = svn_client_mkdir4(
		aprPaths->Handle,
		args->CreateParents,
		nullptr, nullptr, nullptr,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r, paths);
}

bool SvnClient::RemoteCreateDirectory(Uri^ uri)
{
	if (!uri)
		throw gcnew ArgumentNullException("uri");

	return RemoteCreateDirectory(uri, gcnew SvnCreateDirectoryArgs());
}

bool SvnClient::RemoteCreateDirectory(Uri^ uri, SvnCreateDirectoryArgs^ args)
{
	if (!uri)
		throw gcnew ArgumentNullException("uri");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!SvnBase::IsValidReposUri(uri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

	SvnCommitResult^ result;

	return RemoteCreateDirectory(uri, args, result);
}

bool SvnClient::RemoteCreateDirectory(Uri^ uri, SvnCreateDirectoryArgs^ args, [Out] SvnCommitResult^% result)
{
	if (!uri)
		throw gcnew ArgumentNullException("uri");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!SvnBase::IsValidReposUri(uri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

	return RemoteCreateDirectories(NewSingleItemCollection(uri), args, result);
}

bool SvnClient::RemoteCreateDirectories(ICollection<Uri^>^ uris, SvnCreateDirectoryArgs^ args)
{
	if (!uris)
		throw gcnew ArgumentNullException("uris");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitResult^ result;

	return RemoteCreateDirectories(uris, args, result);

}

bool SvnClient::RemoteCreateDirectories(ICollection<Uri^>^ uris, SvnCreateDirectoryArgs^ args, [Out] SvnCommitResult^% result)
{
	if (!uris)
		throw gcnew ArgumentNullException("uris");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	result = nullptr;

	array<String^>^ uriData = gcnew array<String^>(uris->Count);
	int i = 0;

	for each (Uri^ uri in uris)
	{
		if (uri == nullptr)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "uris");
		else if (!SvnBase::IsValidReposUri(uri))
			throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uris");

		uriData[i++] = UriToCanonicalString(uri);
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);
	CommitResultReceiver crr(this);

	AprArray<String^, AprUriMarshaller^>^ aprPaths = gcnew AprArray<String^, AprUriMarshaller^>(safe_cast<ICollection<String^>^>(uriData), %pool);

	svn_error_t *r = svn_client_mkdir4(
		aprPaths->Handle,
		args->CreateParents,
		CreateRevPropList(args->LogProperties, %pool),
		crr.CommitCallback, crr.CommitBaton,
		CtxHandle,
		pool.Handle);

	result = crr.CommitResult;

	return args->HandleResult(this, r, uris);
}
