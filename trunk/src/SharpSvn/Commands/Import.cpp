// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/CheckOut.h"
#include "Args/Import.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Import(System.String,System.Uri,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Import(System.String,System.Uri,SharpSvn.SvnImportArgs,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteImport(System.String,System.Uri,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteImport(System.String,System.Uri,SharpSvn.SvnImportArgs,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="3#")];

bool SvnClient::Import(String^ path, Uri^ target)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!target)
		throw gcnew ArgumentNullException("target");

	return Import(path, target, gcnew SvnImportArgs());
}

bool SvnClient::Import(String^ path, Uri^ target, [Out] SvnCommitResult^% result)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!target)
		throw gcnew ArgumentNullException("target");

	return Import(path, target, gcnew SvnImportArgs(), result);
}

bool SvnClient::Import(String^ path, Uri^ target, SvnImportArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitResult^ result;

	return Import(path, target, args, result);
}

bool SvnClient::Import(String^ path, Uri^ target, SvnImportArgs^ args, [Out] SvnCommitResult^% result)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!SvnBase::IsValidReposUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "target");

	if (RemoteImport(path, target, args, result))
	{
		SvnCheckOutArgs^ aa = gcnew SvnCheckOutArgs();
		aa->ThrowOnError = args->ThrowOnError;
		aa->Depth = args->Depth;

		aa->AllowObstructions = true; // This is the trick
		aa->SvnError += gcnew EventHandler<SvnErrorEventArgs^>(args, &SvnImportArgs::HandleOnSvnError);

		try
		{
			return CheckOut(target, path, aa);
		}
		finally
		{
			args->LastException = aa->LastException;
		}
	}
	else
		return false;
}

bool SvnClient::RemoteImport(String^ path, Uri^ target)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!target)
		throw gcnew ArgumentNullException("target");

	return RemoteImport(path, target, gcnew SvnImportArgs());
}

bool SvnClient::RemoteImport(String^ path, Uri^ target, [Out] SvnCommitResult^% result)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!target)
		throw gcnew ArgumentNullException("target");

	return RemoteImport(path, target, gcnew SvnImportArgs(), result);
}

bool SvnClient::RemoteImport(String^ path, Uri^ target, SvnImportArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitResult^ result;

	return RemoteImport(path, target, args, result);
}

bool SvnClient::RemoteImport(String^ path, Uri^ target, SvnImportArgs^ args, [Out] SvnCommitResult^% result)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!SvnBase::IsValidReposUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "target");

	result = nullptr;

	EnsureState(SvnContextState::AuthorizationInitialized, SvnExtendedState::MimeTypesLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_commit_info_t *commitInfoPtr = nullptr;

	svn_error_t *r = svn_client_import3(
		&commitInfoPtr,
		pool.AllocPath(path),
		pool.AllocCanonical(target),
		(svn_depth_t)args->Depth,
		args->NoIgnore,
		args->IgnoreUnknownNodeTypes,
		CreateRevPropList(args->LogProperties, %pool),
		CtxHandle,
		pool.Handle);

	if (commitInfoPtr)
		result = SvnCommitResult::Create(this, args, commitInfoPtr, %pool);
	else
		result = nullptr;

	return args->HandleResult(this, r);
}
