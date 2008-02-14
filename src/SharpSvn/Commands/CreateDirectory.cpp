// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteCreateDirectories(System.Collections.Generic.ICollection`1<System.Uri>,SharpSvn.SvnCreateDirectoryArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="2#")]
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteCreateDirectory(System.Uri,SharpSvn.SvnCreateDirectoryArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="2#")];

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
	else if(!args)
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
	svn_commit_info_t* commitInfo = nullptr;

	svn_error_t *r = svn_client_mkdir3(
		&commitInfo,
		aprPaths->Handle,
		args->MakeParents,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
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
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!SvnBase::IsValidReposUri(uri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

	SvnCommitInfo^ commitInfo;

	return RemoteCreateDirectory(uri, args, commitInfo);
}

bool SvnClient::RemoteCreateDirectory(Uri^ uri, SvnCreateDirectoryArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if (!uri)
		throw gcnew ArgumentNullException("uri");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!SvnBase::IsValidReposUri(uri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uri");

	return RemoteCreateDirectories(NewSingleItemCollection(uri), args, commitInfo);
}

bool SvnClient::RemoteCreateDirectories(ICollection<Uri^>^ uris, SvnCreateDirectoryArgs^ args)
{
	if (!uris)
		throw gcnew ArgumentNullException("uris");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitInfo^ commitInfo;

	return RemoteCreateDirectories(uris, args, commitInfo);

}

bool SvnClient::RemoteCreateDirectories(ICollection<Uri^>^ uris, SvnCreateDirectoryArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if (!uris)
		throw gcnew ArgumentNullException("uris");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	commitInfo = nullptr;

	array<String^>^ uriData = gcnew array<String^>(uris->Count);
	int i = 0;

	for each (Uri^ uri in uris)
	{
		if (uri == nullptr)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "uris");
		else if(!SvnBase::IsValidReposUri(uri))
			throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "uris");

		uriData[i++] = uri->ToString();
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprArray<String^, AprCanonicalMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCanonicalMarshaller^>(safe_cast<ICollection<String^>^>(uriData), %pool);
	svn_commit_info_t* commit_info = nullptr;

	svn_error_t *r = svn_client_mkdir3(
		&commit_info,
		aprPaths->Handle,
		args->MakeParents,
		CtxHandle,
		pool.Handle);

	if (commit_info)
		commitInfo = gcnew SvnCommitInfo(commit_info, %pool);

	return args->HandleResult(this, r);
}
