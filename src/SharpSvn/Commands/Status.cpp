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

static svn_error_t* svnclient_status_handler(void *baton, const char *path, const svn_client_status_t *status, apr_pool_t *scratch_pool)
{
    SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

    AprPool aprPool(scratch_pool, false);
    SvnStatusArgs^ args = dynamic_cast<SvnStatusArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs

    if (!args)
        return nullptr;

    SvnStatusEventArgs^ e = gcnew SvnStatusEventArgs(path, status, client, %aprPool);

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

    if (args->RetrieveRemoteStatus)
        EnsureState(SvnContextState::AuthorizationInitialized);
    else
        EnsureState(SvnContextState::ConfigLoaded);

    AprPool pool(%_pool);
    ArgsStore store(this, args, %pool);

    if (statusHandler)
        args->Status += statusHandler;
    try
    {
        svn_revnum_t version = 0;

        svn_opt_revision_t pegRev = args->Revision->ToSvnRevision();

        svn_error_t* r = svn_client_status6(&version,
            CtxHandle,
            pool.AllocDirent(path),
            &pegRev,
            (svn_depth_t)args->Depth,
            args->RetrieveAllEntries,
            args->RetrieveRemoteStatus,
            !args->IgnoreWorkingCopyStatus,
            args->RetrieveIgnoredEntries,
            args->IgnoreExternals,
            args->KeepDepth,
            CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
            svnclient_status_handler,
            (void*)_clientBaton->Handle,
            pool.Handle);

        return args->HandleResult(this, r, path);
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

#include "private/svn_wc_private.h"

SvnWorkingCopyInfo::SvnWorkingCopyInfo(const svn_client_status_t *status, SvnClientContext^ client, AprPool^ pool)
{
    if (!status)
        throw gcnew ArgumentNullException("status");
    else if (!client)
        throw gcnew ArgumentNullException("client");
    else if (!pool)
        throw gcnew ArgumentNullException("pool");

    _client = client;
    _status = status;
    _pool = pool;
}

void SvnWorkingCopyInfo::Ensure()
{
    if (_ensured || !_status)
        return;

    _ensured = true;

#pragma warning(push)
#pragma warning(disable: 4996)
    svn_wc_status2_t *status2;
    SVN_THROW(svn_wc__status2_from_3(&status2, (const svn_wc_status3_t*)_status->backwards_compatibility_baton,
                                                                     _client->CtxHandle->wc_ctx,
                                                                     _status->local_abspath, _pool->Handle, _pool->Handle));
#pragma warning(pop)

    const svn_wc_entry_t *entry = status2->entry;
    _entry = entry;

    _revision = entry->revision;
    _nodeKind = (SvnNodeKind)entry->kind;
    _schedule = (SvnSchedule)entry->schedule;
    _copied = (entry->copied != 0);
    _deleted = (entry->deleted != 0);
    _absent = (entry->absent != 0);
    _incomplete = (entry->incomplete != 0);
    _copyFromRev = entry->copyfrom_rev;
    _textTime = SvnBase::DateTimeFromAprTime(entry->text_time);
    _lastChangeRev = entry->cmt_rev;
    _lastChangeTime = SvnBase::DateTimeFromAprTime(entry->cmt_date);
    _lockTime = SvnBase::DateTimeFromAprTime(entry->lock_creation_date);
    _hasProperties = (entry->has_props != 0);
    _hasPropertyChanges = (entry->has_prop_mods != 0);
    _wcSize = entry->working_size;
    _keepLocal = (entry->keep_local != 0);
    _depth = (SvnDepth)entry->depth;
}
