// Copyright 2007-2025 The SharpSvn Project
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

#include "Args/List.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetList(SharpSvn.SvnTarget,SharpSvn.SvnListArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnListEventArgs>&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetList(SharpSvn.SvnTarget,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnListEventArgs>&)", MessageId="1#")];

bool SvnClient::List(SvnTarget^ target, EventHandler<SvnListEventArgs^>^ listHandler)
{
    if (!target)
        throw gcnew ArgumentNullException("target");
    else if (!listHandler)
        throw gcnew ArgumentNullException("listHandler");

    return List(target, gcnew SvnListArgs(), listHandler);
}

static svn_error_t *svnclient_list_handler(void *baton,
                                                                                   const char *path,
                                                                                   const svn_dirent_t *dirent,
                                                                                   const svn_lock_t *lock,
                                                                                   const char *abs_path,
                                                                                   const char *external_parent_url,
                                                                                   const char *external_target,
                                                                                   apr_pool_t *pool)
{
    SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

    AprPool thePool(pool, false);

    SvnListArgs^ args = dynamic_cast<SvnListArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs
    if (!args)
        return nullptr;

    SvnListEventArgs^ e = gcnew SvnListEventArgs(path, dirent, lock, abs_path, args->CalculateRepositoryRoot(abs_path),
                                                                                             external_parent_url, external_target);
    try
    {
        args->OnList(e);

        if (e->Cancel)
            return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "List receiver canceled operation");
        else
            return nullptr;
    }
    catch(Exception^ ex)
    {
        return SvnException::CreateExceptionSvnError("List receiver", ex);
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
    else if (!args)
        throw gcnew ArgumentNullException("args");

    // We allow a null listHandler; the args object might just handle it itself

    EnsureState(SvnContextState::AuthorizationInitialized);
    AprPool pool(%_pool);
    ArgsStore store(this, args, %pool);

    args->Prepare(target, args->Revision->RevisionType != SvnRevisionType::None);

    if (listHandler)
        args->List += listHandler;
    try
    {
        svn_opt_revision_t pegrev = target->Revision->ToSvnRevision();
        svn_opt_revision_t rev = args->Revision->Or(target->Revision)->ToSvnRevision();

        svn_error_t* r = svn_client_list4(
            target->AllocAsString(%pool),
            &pegrev,
            &rev,
            nullptr /* patterns */,
            (svn_depth_t)args->Depth,
            (apr_uint32_t)args->RetrieveEntries,
            args->RetrieveLocks,
            args->IncludeExternals,
            svnclient_list_handler,
            (void*)_clientBaton->Handle,
            CtxHandle,
            pool.Handle);

        return args->HandleResult(this, r, target);
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
    else if (!args)
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
