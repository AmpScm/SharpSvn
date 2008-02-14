// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;


[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetList(SharpSvn.SvnTarget,SharpSvn.SvnListArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnListEventArgs>&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetList(SharpSvn.SvnTarget,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnListEventArgs>&)", MessageId="1#")];
bool SvnClient::List(SvnTarget^ target, EventHandler<SvnListEventArgs^>^ listHandler)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(!listHandler)
		throw gcnew ArgumentNullException("listHandler");

	return List(target, gcnew SvnListArgs(), listHandler);
}

static svn_error_t *svnclient_list_handler(void *baton, const char *path, const svn_dirent_t *dirent, const svn_lock_t *lock, const char *abs_path, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	AprPool thePool(pool, false);

	SvnListArgs^ args = dynamic_cast<SvnListArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs
	if (!args)
		return nullptr;

	SvnListEventArgs^ e = gcnew SvnListEventArgs(path, dirent, lock, abs_path);
	try
	{
		args->OnList(e);

		if (e->Cancel)
			return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "List receiver canceled operation");
		else
			return nullptr;
	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("List receiver", e);
	}
	finally
	{
		e->Detach(false);
	}
}

bool SvnClient::List(SvnTarget^ target, SvnListArgs^ args, EventHandler<SvnListEventArgs^>^ listHandler)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	// We allow a null listHandler; the args object might just handle it itself

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if (listHandler)
		args->List += listHandler;
	try
	{
		svn_opt_revision_t pegrev = target->Revision->ToSvnRevision();
		svn_opt_revision_t rev = args->Revision->ToSvnRevision();

		svn_error_t* r = svn_client_list2(
			pool.AllocString(target->TargetName),
			&pegrev,
			&rev,
			(svn_depth_t)args->Depth,
			(apr_uint32_t)args->EntryItems,
			args->FetchLocks,
			svnclient_list_handler,
			(void*)_clientBatton->Handle,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(this, r);
	}
	finally
	{
		if (listHandler)
			args->List -= listHandler;
	}
}

bool SvnClient::GetList(SvnTarget^ target, [Out] Collection<SvnListEventArgs^>^% list)
{
	if (!target)
		throw gcnew ArgumentNullException("target");

	InfoItemCollection<SvnListEventArgs^>^ results = gcnew InfoItemCollection<SvnListEventArgs^>();

	try
	{
		return List(target, gcnew SvnListArgs(), results->Handler);
	}
	finally
	{
		list = results;
	}
}


bool SvnClient::GetList(SvnTarget^ target, SvnListArgs^ args, [Out] Collection<SvnListEventArgs^>^% list)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnListEventArgs^>^ results = gcnew InfoItemCollection<SvnListEventArgs^>();

	try
	{
		return List(target, args, results->Handler);
	}
	finally
	{
		list = results;
	}
}
