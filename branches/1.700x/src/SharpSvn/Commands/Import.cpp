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
			return CheckOut(gcnew SvnUriTarget(target, result->Revision), path, aa);
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

static svn_error_t *
sharpsvn_import_filter(void *baton, svn_boolean_t *filtered, const char *local_abspath, const svn_io_dirent2_t *dirent, apr_pool_t *scratch_pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	AprPool thePool(scratch_pool, false);

	SvnImportArgs^ args = dynamic_cast<SvnImportArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs
	if (!args)
		return nullptr;

	SvnImportFilterEventArgs^ e = gcnew SvnImportFilterEventArgs(local_abspath, dirent, %thePool);
	try
	{
		args->OnFilter(e);

		*filtered = e->Filter ? TRUE : FALSE;

		if (e->Cancel)
			return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Import filter canceled operation");
		else
			return nullptr;
	}
	catch(Exception^ ex)
	{
		return SvnException::CreateExceptionSvnError("Import filter", ex);
	}
	finally
	{
		e->Detach(false);
	}
}

bool SvnClient::RemoteImport(String^ path, Uri^ target, SvnImportArgs^ args, [Out] SvnCommitResult^% result)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!SvnBase::IsValidReposUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "target");

	result = nullptr;

	EnsureState(SvnContextState::AuthorizationInitialized, SvnExtendedState::MimeTypesLoaded);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);
	CommitResultReceiver crr(this);

	svn_error_t *r = svn_client_import5(
		pool.AllocDirent(path),
		pool.AllocUri(target),
		(svn_depth_t)args->Depth,
		args->NoIgnore,
		args->IgnoreUnknownNodeTypes,
		CreateRevPropList(args->LogProperties, %pool),
		args->HasFilter ? sharpsvn_import_filter : nullptr, (void*)_clientBaton->Handle,
		crr.CommitCallback, crr.CommitBaton,
		CtxHandle,
		pool.Handle);

	result = crr.CommitResult;

	return args->HandleResult(this, r, path);
}
