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
#include "Args/Status.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetStatus(System.String,SharpSvn.SvnStatusArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnStatusEventArgs>&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetStatus(System.String,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnStatusEventArgs>&)", MessageId="1#")];

bool SvnClient::Status(String^ path, EventHandler<SvnStatusEventArgs^>^ statusHandler)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if (!statusHandler)
		throw gcnew ArgumentNullException("statusHandler");

	return Status(path, gcnew SvnStatusArgs(), statusHandler);
}

svn_error_t* svnclient_status_handler(void *baton, const char *path, svn_wc_status2_t *status, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	AprPool aprPool(pool, false);
	SvnStatusArgs^ args = dynamic_cast<SvnStatusArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs

	if (!args)
		return nullptr;

	SvnStatusEventArgs^ e = gcnew SvnStatusEventArgs(
		SvnBase::Utf8_PathPtrToString(path, %aprPool), status, %aprPool);

	try
	{
		args->OnStatus(e);

		if (e->Cancel)
			return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Status receiver canceled operation");
		else
			return nullptr;
	}
	catch(Exception^ ex)
	{
		return SvnException::CreateExceptionSvnError("Status receiver", ex);
	}
	finally
	{
		e->Detach(false);
	}
}

bool SvnClient::Status(String^ path, SvnStatusArgs^ args, EventHandler<SvnStatusEventArgs^>^ statusHandler)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	// We allow a null statusHandler; the args object might just handle it itself

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if (statusHandler)
		args->Status += statusHandler;
	try
	{
		svn_revnum_t version = 0;

		svn_opt_revision_t pegRev = args->Revision->ToSvnRevision();

		svn_client_status_args_t *status_args = svn_client_status_args_create(pool.Handle);

		status_args->get_all = args->RetrieveAllEntries;
		status_args->no_ignore = args->RetrieveIgnoredEntries;
		status_args->ignore_externals = args->IgnoreExternals;

		svn_error_t* r = svn_client_status4(&version,
			pool.AllocPath(path),
			&pegRev,
			svnclient_status_handler,
			(void*)_clientBatton->Handle,
			(svn_depth_t)args->Depth,
			args->ContactRepository,
			status_args,
			CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
			CtxHandle,
			pool.Handle);

		return args->HandleResult(this, r);
	}
	finally
	{
		if (statusHandler)
			args->Status -= statusHandler;
	}
}

bool SvnClient::GetStatus(String^ path, [Out] Collection<SvnStatusEventArgs^>^% statuses)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	InfoItemCollection<SvnStatusEventArgs^>^ results = gcnew InfoItemCollection<SvnStatusEventArgs^>();

	try
	{
		return Status(path, gcnew SvnStatusArgs(), results->Handler);
	}
	finally
	{
		statuses = results;
	}
}

bool SvnClient::GetStatus(String^ path, SvnStatusArgs^ args, [Out] Collection<SvnStatusEventArgs^>^% statuses)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnStatusEventArgs^>^ results = gcnew InfoItemCollection<SvnStatusEventArgs^>();

	try
	{
		return Status(path, args, results->Handler);
	}
	finally
	{
		statuses = results;
	}
}
