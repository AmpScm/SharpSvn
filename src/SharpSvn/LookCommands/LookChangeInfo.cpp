// Copyright 2008-2009 The SharpSvn Project
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

#include "LookArgs/SvnChangeInfoArgs.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types


[module:SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope = "member", Target = "SharpSvn.SvnLookClient.#GetChangeInfo(SharpSvn.SvnLookOrigin,SharpSvn.SvnChangeInfoArgs,SharpSvn.SvnChangeInfoEventArgs&)", MessageId = "2#")];
[module:SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope = "member", Target = "SharpSvn.SvnLookClient.#GetChangeInfo(SharpSvn.SvnLookOrigin,SharpSvn.SvnChangeInfoEventArgs&)", MessageId = "1#")] ;

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnLookClient::ChangeInfo(SvnLookOrigin^ lookOrigin, EventHandler<SvnChangeInfoEventArgs^>^ changeInfoHandler)
{
    if (!lookOrigin)
        throw gcnew ArgumentNullException("lookOrigin");
    else if (!changeInfoHandler)
        throw gcnew ArgumentNullException("changeInfoHandler");

    return ChangeInfo(lookOrigin, gcnew SvnChangeInfoArgs(), changeInfoHandler);
}

static const char* create_name(svn_repos_node_t* node, AprPool^ pool)
{
    if (!node)
        return "";
    else if (!node->parent)
        return "/";
    else if (!node->parent->name || !node->parent->name[0])
        return apr_pstrcat(pool->Handle, "/", node->name, (const char*)nullptr);
    else
        return apr_pstrcat(pool->Handle, create_name(node->parent, pool), "/", node->name, (const char*)nullptr);
}

static void create_changes_hash(apr_hash_t* ht, svn_repos_node_t* node, AprPool^ pool, AprPool^ tmpPool)
{
    if (!ht)
        throw gcnew ArgumentNullException("ht");
    else if (!node)
        throw gcnew ArgumentNullException("node");
    else if (!pool)
        throw gcnew ArgumentNullException("pool");

    if (node->action != 'R' || node->text_mod || node->prop_mod || node->copyfrom_path)
    {
        svn_log_changed_path2_t* chg = (svn_log_changed_path2_t*)svn_log_changed_path2_create(pool->Handle);

        if (node->action == 'R' && !node->copyfrom_path)
            chg->action = 'M';
        else
            chg->action = node->action;

        chg->copyfrom_path = node->copyfrom_path;
        chg->copyfrom_rev = node->copyfrom_rev;

        const char* name = node->parent ? apr_pstrdup(pool->Handle, create_name(node, tmpPool)) : node->name;

        if (!*name)
            name = "/"; // Static char* "/" for the repository root

        apr_hash_set(ht, name, APR_HASH_KEY_STRING, chg); // TODO: Check if name is valid! (maybe use parents names as suffix!)

        chg->node_kind = node->kind;
    }

    if (node->child)
        create_changes_hash(ht, node->child, pool, tmpPool);

    if (node->sibling)
        create_changes_hash(ht, node->sibling, pool, tmpPool);
}

bool SvnLookClient::ChangeInfo(SvnLookOrigin^ lookOrigin, SvnChangeInfoArgs^ args, EventHandler<SvnChangeInfoEventArgs^>^ changeInfoHandler)
{
    if (!lookOrigin)
        throw gcnew ArgumentNullException("lookOrigin");
    else if (!args)
        throw gcnew ArgumentNullException("args");

    EnsureState(SvnContextState::ConfigLoaded);
    AprPool pool(% _pool);
    ArgsStore store(this, args, % pool);

    if (changeInfoHandler)
        args->ChangeInfo += changeInfoHandler;

    try
    {
        svn_log_entry_t* entry = svn_log_entry_create(pool.Handle);

        svn_repos_t* repos = nullptr;
        svn_fs_t* fs;
        svn_error_t* r;
        apr_hash_t* props = nullptr;

        svn_revnum_t base_rev;
        svn_fs_root_t* root = nullptr;

        if (r = svn_repos_open3(
            &repos,
            pool.AllocDirent(lookOrigin->RepositoryPath),
            nullptr,
            pool.Handle, pool.Handle))
        {
            return args->HandleResult(this, r);
        }

        fs = svn_repos_fs(repos);

        if (lookOrigin->HasTransaction)
        {
            svn_fs_txn_t* txn = nullptr;

            if (r = svn_fs_open_txn(&txn, fs, pool.AllocString(lookOrigin->Transaction), pool.Handle))
                return args->HandleResult(this, r);

            if (r = svn_fs_txn_proplist(&props, txn, pool.Handle))
                return args->HandleResult(this, r);

            entry->revision = -1;
            base_rev = svn_fs_txn_base_revision(txn);

            if (args->RetrieveChangedPaths)
            {
                if (r = svn_fs_txn_root(&root, txn, pool.Handle))
                    return args->HandleResult(this, r);
            }
        }
        else
        {
            svn_revnum_t rev;

            if (!lookOrigin->HasRevision)
            {
                if (r = svn_fs_youngest_rev(&rev, fs, pool.Handle))
                    return args->HandleResult(this, r);
            }
            else
                rev = (svn_revnum_t)lookOrigin->Revision;

            if (r = svn_fs_revision_proplist2(&props, fs, rev, !args->NoRefresh, pool.Handle, pool.Handle))
            {
                return args->HandleResult(this, r);
            }

            entry->revision = rev;
            base_rev = rev - 1;

            if (args->RetrieveChangedPaths)
            {
                if (r = svn_fs_revision_root(&root, fs, rev, pool.Handle))
                    return args->HandleResult(this, r);
            }
        }

        entry->revprops = props;

        if (args->RetrieveChangedPaths)
        {
            System::Diagnostics::Debug::Assert(root != nullptr);

            svn_repos_node_t* tree;
            {
                AprPool tmpPool(% pool);
                void* edit_baton;

                svn_fs_root_t* base_root;
                if (r = svn_fs_revision_root(&base_root, fs, base_rev, tmpPool.Handle))
                    return args->HandleResult(this, r);

                const svn_delta_editor_t* editor;

                if (r = svn_repos_node_editor(
                    &editor,
                    &edit_baton,
                    repos,
                    base_root,
                    root,
                    pool.Handle,
                    tmpPool.Handle))
                {
                    return args->HandleResult(this, r);
                }

                if (r = svn_repos_replay2(
                    root,
                    "",
                    SVN_INVALID_REVNUM,
                    false,
                    editor,
                    edit_baton,
                    nullptr,
                    nullptr,
                    tmpPool.Handle))
                {
                    return args->HandleResult(this, r);
                }

                tree = svn_repos_node_from_baton(edit_baton);

                apr_hash_t* changes = apr_hash_make(pool.Handle);

                create_changes_hash(changes, tree, % pool, % tmpPool);

                entry->changed_paths2 = entry->changed_paths = changes;
            }
        }

        SvnChangeInfoEventArgs^ e = gcnew SvnChangeInfoEventArgs(entry, base_rev, % pool);
        try
        {
            args->OnChangeInfo(e);
        }
        finally
        {
            e->Detach(false);
        }

        return args->HandleResult(this, r);
    }
    finally
    {
        if (changeInfoHandler)
            args->ChangeInfo -= changeInfoHandler;
    }
}

bool SvnLookClient::GetChangeInfo(SvnLookOrigin^ lookOrigin, [Out] SvnChangeInfoEventArgs^% changeInfo)
{
    if (!lookOrigin)
        throw gcnew ArgumentNullException("lookOrigin");

    return GetChangeInfo(lookOrigin, gcnew SvnChangeInfoArgs(), changeInfo);
}

bool SvnLookClient::GetChangeInfo(SvnLookOrigin^ lookOrigin, SvnChangeInfoArgs^ args, [Out] SvnChangeInfoEventArgs^% changeInfo)
{
    if (!lookOrigin)
        throw gcnew ArgumentNullException("lookOrigin");
    else if (!args)
        throw gcnew ArgumentNullException("args");

    SingleInfoItemReceiver<SvnChangeInfoEventArgs^>^ result = gcnew SingleInfoItemReceiver<SvnChangeInfoEventArgs^>();

    try
    {
        return ChangeInfo(lookOrigin, args, result->Handler);
    }
    finally
    {
        changeInfo = result->Value;
    }
}
