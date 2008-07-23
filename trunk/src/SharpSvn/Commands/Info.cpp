// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/Info.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;


[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetInfo(SharpSvn.SvnTarget,SharpSvn.SvnInfoEventArgs&)", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetInfo(SharpSvn.SvnTarget,SharpSvn.SvnInfoArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnInfoEventArgs>&)", MessageId="2#")];

bool SvnClient::Info(SvnTarget^ target, EventHandler<SvnInfoEventArgs^>^ infoHandler)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!infoHandler)
		throw gcnew ArgumentNullException("infoHandler");

	return Info(target, gcnew SvnInfoArgs(), infoHandler);
}

static svn_error_t* svn_info_receiver(void *baton, const char *path, const svn_info_t *info, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	AprPool thePool(pool, false);
	SvnInfoArgs^ args = dynamic_cast<SvnInfoArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs
	if (!args)
		return nullptr;

	SvnInfoEventArgs^ e = gcnew SvnInfoEventArgs(SvnBase::Utf8_PtrToString(path), info);
	try
	{
		args->OnInfo(e);

		if (e->Cancel)
			return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Info receiver canceled operation");
		else
			return nullptr;
	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("Info receiver", e);
	}
	finally
	{
		e->Detach(false);
	}
}

bool SvnClient::Info(SvnTarget^ target, SvnInfoArgs^ args, EventHandler<SvnInfoEventArgs^>^ infoHandler)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	// We allow a null infoHandler; the args object might just handle it itself

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if (infoHandler)
		args->Info += infoHandler;
	try
	{
		svn_opt_revision_t pegRev = target->Revision->ToSvnRevision();
		svn_opt_revision_t rev = args->Revision->Or(target->GetSvnRevision(SvnRevision::None, SvnRevision::Head))->ToSvnRevision();

		svn_error_t* r = svn_client_info2(
			pool.AllocString(target->SvnTargetName),
			&pegRev,
			&rev,
			svn_info_receiver,
			(void*)_clientBatton->Handle,
			(svn_depth_t)args->Depth,
			CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
			CtxHandle,
			pool.Handle);

		return args->HandleResult(this, r);
	}
	finally
	{
		if (infoHandler)
			args->Info -= infoHandler;
	}
}

bool SvnClient::GetInfo(SvnTarget^ target, [Out] SvnInfoEventArgs^% info)
{
	if (!target)
		throw gcnew ArgumentNullException("target");

	InfoItemCollection<SvnInfoEventArgs^>^ results = gcnew InfoItemCollection<SvnInfoEventArgs^>();

	try
	{
		return Info(target, gcnew SvnInfoArgs(), results->Handler);
	}
	finally
	{
		if (results->Count > 0)
			info = results[0];
		else
			info = nullptr;
	}
}

bool SvnClient::GetInfo(SvnTarget^ target, SvnInfoArgs^ args, [Out] Collection<SvnInfoEventArgs^>^% info)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnInfoEventArgs^>^ results = gcnew InfoItemCollection<SvnInfoEventArgs^>();

	try
	{
		return Info(target, args, results->Handler);
	}
	finally
	{
		info = results;
	}
}
