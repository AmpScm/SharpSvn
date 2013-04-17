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

static svn_error_t* svn_info_receiver(void *baton, const char *path, const svn_client_info2_t *info, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	AprPool thePool(pool, false);
	SvnInfoArgs^ args = dynamic_cast<SvnInfoArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs
	if (!args)
		return nullptr;

	SvnInfoEventArgs^ e = gcnew SvnInfoEventArgs(SvnBase::Utf8_PathPtrToString(path, %thePool), info, %thePool);
	try
	{
		args->OnInfo(e);

		if (e->Cancel)
			return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Info receiver canceled operation");
		else
			return nullptr;
	}
	catch(Exception^ ex)
	{
		return SvnException::CreateExceptionSvnError("Info receiver", ex);
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
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	if (infoHandler)
		args->Info += infoHandler;
	try
	{
		svn_opt_revision_t pegRev = target->GetSvnRevision(SvnRevision::None, SvnRevision::Head)->ToSvnRevision();
		svn_opt_revision_t rev = args->Revision->Or(target->GetSvnRevision(SvnRevision::None, SvnRevision::Head))->ToSvnRevision();

		svn_error_t* r = svn_client_info3(
			target->AllocAsString(%pool, true),
			&pegRev,
			&rev,
			(svn_depth_t)args->Depth,
			args->RetrieveExcluded,
			args->RetrieveActualOnly,
			CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
			svn_info_receiver,
			(void*)_clientBaton->Handle,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(this, r, target);
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

ICollection<SvnConflictData^>^ SvnInfoEventArgs::Conflicts::get()
{
    if (!_conflicted || _conflicts || !_info || !_info->wc_info)
        return _conflicts;

    List<SvnConflictData^>^ items = gcnew List<SvnConflictData^>();

	for (int i = 0; i < _info->wc_info->conflicts->nelts; i++)
    {
        svn_wc_conflict_description2_t *c = APR_ARRAY_IDX(_info->wc_info->conflicts, i, svn_wc_conflict_description2_t *);

        items->Add(gcnew SvnConflictData(c, _pool));
    }

    return _conflicts = items->AsReadOnly();
}
