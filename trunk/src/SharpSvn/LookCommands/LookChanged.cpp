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
#include "SvnAll.h"
#include "LookArgs/SvnChangedArgs.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnLookClient.#GetChanged(SharpSvn.SvnLookOrigin,SharpSvn.SvnChangedArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnChangedEventArgs>&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnLookClient.#GetChanged(SharpSvn.SvnLookOrigin,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnChangedEventArgs>&)", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1031:DoNotCatchGeneralExceptionTypes", Scope="member", Target="SharpSvn.SvnLookClient.#ProcessTree(svn_repos_node_t*,System.String,SharpSvn.SvnChangedArgs)")];

bool SvnLookClient::Changed(SvnLookOrigin^ lookOrigin, EventHandler<SvnChangedEventArgs^>^ changedHandler)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if(!changedHandler)
		throw gcnew ArgumentNullException("changedHandler");

	return Changed(lookOrigin, gcnew SvnChangedArgs(), changedHandler);
}

bool SvnLookClient::Changed(SvnLookOrigin^ lookOrigin, SvnChangedArgs^ args, EventHandler<SvnChangedEventArgs^>^ changedHandler)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	if (changedHandler)
		args->Changed += changedHandler;

	try
	{
		svn_error_t* r;
		svn_repos_t* repos;
		if (r = svn_repos_open2(&repos, pool.AllocDirent(lookOrigin->RepositoryPath), nullptr, pool.Handle))
			return args->HandleResult(this, r);

		svn_fs_t* fs = svn_repos_fs(repos);
		svn_revnum_t base_rev;
		svn_fs_txn_t* txn = NULL;
		svn_fs_root_t* root;

		// Figure out whether they are asking for a specific transaction or
		// revision
		if (lookOrigin->HasTransaction)
		{
			if (r = svn_fs_open_txn(&txn, fs, pool.AllocString(lookOrigin->Transaction), pool.Handle))
				return args->HandleResult(this, r);

			base_rev = svn_fs_txn_base_revision(txn);

			if (r = svn_fs_txn_root(&root, txn, pool.Handle))
				return args->HandleResult(this, r);
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
			{
				rev = (svn_revnum_t)lookOrigin->Revision;
			}

			if (r = svn_fs_revision_root(&root, fs, rev, pool.Handle))
				return args->HandleResult(this, r);

			base_rev = rev - 1;
		}

		if (!SVN_IS_VALID_REVNUM(base_rev))
		{
			throw gcnew ArgumentException("Transaction " + args->Transaction + " not based on a revision");
		}

		svn_fs_root_t* base_root;
		if (r = svn_fs_revision_root(&base_root, fs, base_rev, pool.Handle))
			return args->HandleResult(this, r);

		const svn_delta_editor_t *editor;
		void *edit_baton;

		AprPool edit_pool(%_pool);

		if (r = svn_repos_node_editor(
					&editor,
					&edit_baton,
					repos,
					base_root,
					root,
					edit_pool.Handle,
					pool.Handle
			))
			return args->HandleResult(this, r);

		if (r = svn_repos_replay2(
					root,
					"",
					SVN_INVALID_REVNUM,
					FALSE,
					editor,
					edit_baton,
					NULL,
					NULL,
					edit_pool.Handle
			))
			return args->HandleResult(this, r);

		svn_repos_node_t* tree = svn_repos_node_from_baton(edit_baton);
		r = ProcessTree(tree, nullptr, args);

		return args->HandleResult(this, r);
	}
	finally
	{
		if (changedHandler)
			args->Changed -= changedHandler;
	}
}

svn_error_t* SvnLookClient::ProcessTree(svn_repos_node_t *node, String^ basePath, SvnChangedArgs^ args)
{
	if (!node)
		throw gcnew ArgumentNullException("node");

	String^ name = SvnBase::Utf8_PtrToString(node->name);
	String^ path = basePath ? (basePath + "/" + name) : name;

	if (node->action)
	{
		String^ fp = (((SvnNodeKind)node->kind) == SvnNodeKind::Directory) ? (path + "/") : path;

		SvnChangedEventArgs^ e = gcnew SvnChangedEventArgs(
			fp,
			name,
			(SvnChangeAction)node->action,
			SvnBase::Utf8_PtrToString(node->copyfrom_path),
			node->copyfrom_rev,
			(SvnNodeKind)node->kind,
			node->prop_mod ? true : false,
			node->text_mod ? true : false
		);

		try
		{
			args->OnChanged(e); // Send to receiver

			if (e->Cancel)
			{
				return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Changed receiver canceled operation");
			}
		}
		catch (Exception^ ex)
		{
			return SvnException::CreateExceptionSvnError("Changed receiver", ex);
		}
		finally
		{
			e->Detach(false);
		}
	}

	svn_repos_node_t *child = node->child;

	if (!child)
		return nullptr; // No children

	String^ fullpath = node->name[0] ? path : "";

	while(child)
	{
		svn_error_t* r = ProcessTree(child, fullpath, args);

		if (r)
			return r;

		child = child->sibling;
	}

	return nullptr;
}

bool SvnLookClient::GetChanged(SvnLookOrigin^ lookOrigin, [Out] Collection<SvnChangedEventArgs^>^% changedItems)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");

	InfoItemCollection<SvnChangedEventArgs^>^ results = gcnew InfoItemCollection<SvnChangedEventArgs^>();

	try
	{
		return Changed(lookOrigin, gcnew SvnChangedArgs(), results->Handler);
	}
	finally
	{
		changedItems = results;
	}
}

bool SvnLookClient::GetChanged(SvnLookOrigin^ lookOrigin, SvnChangedArgs^ args, [Out] Collection<SvnChangedEventArgs^>^% changedItems)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnChangedEventArgs^>^ results = gcnew InfoItemCollection<SvnChangedEventArgs^>();

	try
	{
		return Changed(lookOrigin, args, results->Handler);
	}
	finally
	{
		changedItems = results;
	}
}
