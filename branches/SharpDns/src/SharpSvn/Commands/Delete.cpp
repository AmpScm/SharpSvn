// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/Delete.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteDelete(System.Collections.Generic.ICollection`1<System.Uri>,SharpSvn.SvnDeleteArgs,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteDelete(System.Uri,SharpSvn.SvnDeleteArgs,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="2#")];

bool SvnClient::Delete(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	return Delete(NewSingleItemCollection(path), gcnew SvnDeleteArgs());
}

bool SvnClient::Delete(String^ path, SvnDeleteArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	return Delete(NewSingleItemCollection(path), args);
}


bool SvnClient::Delete(ICollection<String^>^ paths)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");

	return Delete(paths, gcnew SvnDeleteArgs());
}

bool SvnClient::Delete(ICollection<String^>^ paths, SvnDeleteArgs^ args)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	for each (String^ path in paths)
	{
		if (String::IsNullOrEmpty(path))
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "paths");
		else if(!IsNotUri(path))
			throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "paths");
	}

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprArray<String^, AprCStrPathMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCStrPathMarshaller^>(paths, %pool);
	svn_commit_info_t* result = nullptr;

	svn_error_t *r = svn_client_delete3(
		&result,
		aprPaths->Handle,
		args->Force,
		args->KeepLocal,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}

bool SvnClient::RemoteDelete(Uri^ uri)
{
	if (!uri)
		throw gcnew ArgumentNullException("uri");
	else if(!SvnBase::IsValidReposUri(uri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

	return RemoteDelete(NewSingleItemCollection(uri), gcnew SvnDeleteArgs());
}


bool SvnClient::RemoteDelete(Uri^ uri, SvnDeleteArgs^ args)
{
	if (!uri)
		throw gcnew ArgumentNullException("uri");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!SvnBase::IsValidReposUri(uri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

	return RemoteDelete(NewSingleItemCollection(uri), args);
}

bool SvnClient::RemoteDelete(Uri^ uri, SvnDeleteArgs^ args, [Out] SvnCommitResult^% result)
{
	if (!uri)
		throw gcnew ArgumentNullException("uri");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!SvnBase::IsValidReposUri(uri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

	return RemoteDelete(NewSingleItemCollection(uri), args, result);
}

bool SvnClient::RemoteDelete(ICollection<Uri^>^ uris)
{
	if (!uris)
		throw gcnew ArgumentNullException("uris");

	SvnCommitResult^ result;

	return RemoteDelete(uris, gcnew SvnDeleteArgs(), result);
}


bool SvnClient::RemoteDelete(ICollection<Uri^>^ uris, SvnDeleteArgs^ args)
{
	if (!uris)
		throw gcnew ArgumentNullException("uris");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	SvnCommitResult^ result;

	return RemoteDelete(uris, args, result);
}

bool SvnClient::RemoteDelete(ICollection<Uri^>^ uris, SvnDeleteArgs^ args, [Out] SvnCommitResult^% result)
{
	if (!uris)
		throw gcnew ArgumentNullException("uris");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	result = nullptr;

	array<String^>^ uriData = gcnew array<String^>(uris->Count);
	int i = 0;

	for each (Uri^ uri in uris)
	{
		if (uri == nullptr)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "uris");
		else if(!SvnBase::IsValidReposUri(uri))
			throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uris");

		uriData[i++] = UriToCanonicalString(uri);
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprArray<String^, AprCanonicalMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCanonicalMarshaller^>(safe_cast<ICollection<String^>^>(uriData), %pool);

	svn_commit_info_t* commitInfoPtr = nullptr;

	svn_error_t *r = svn_client_delete3(
		&commitInfoPtr,
		aprPaths->Handle,
		args->Force,
		args->KeepLocal,
		CtxHandle,
		pool.Handle);

	if (commitInfoPtr)
		result = SvnCommitResult::Create(this, args, commitInfoPtr, %pool);
	else
		result = nullptr;

	return args->HandleResult(this, r);
}
