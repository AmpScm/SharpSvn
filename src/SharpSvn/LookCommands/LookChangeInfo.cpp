// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "LookArgs/SvnChangeInfoArgs.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnLookClient::ChangeInfo(String^ repositoryPath, EventHandler<SvnChangeInfoEventArgs^>^ changeInfoHandler)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!changeInfoHandler)
		throw gcnew ArgumentNullException("changeInfoHandler");

	return ChangeInfo(repositoryPath, gcnew SvnChangeInfoArgs(), changeInfoHandler);
}

static const char* create_name(svn_repos_node_t* node, apr_pool_t* pool)
{
	if(!node)
		return "";
	else if(!node->parent)
		return "/";
	else if(!node->parent->name || !node->parent->name[0])
		return apr_pstrcat(pool, "/", node->name, (const char*)nullptr);
	else
		return apr_pstrcat(pool, create_name(node->parent, pool), "/", node->name, (const char*)nullptr);
}

static void create_changes_hash(apr_hash_t* ht, svn_repos_node_t* node, apr_pool_t* pool, apr_pool_t* tmpPool)
{
	if (!ht)
		throw gcnew ArgumentNullException("ht");
	else if (!node)
		throw gcnew ArgumentNullException("node");
	else if (!pool)
		throw gcnew ArgumentNullException("pool");

	if(node->action != 'R' || node->text_mod || node->prop_mod || node->copyfrom_path)
	{
		svn_log_changed_path_t* chg = (svn_log_changed_path_t*)apr_pcalloc(pool, sizeof(svn_log_changed_path_t));

		if(node->action == 'R' && !node->copyfrom_path)
			chg->action = 'M';
		else
			chg->action = node->action;

		chg->copyfrom_path = node->copyfrom_path;
		chg->copyfrom_rev = node->copyfrom_rev;		

		const char* name = node->parent ? apr_pstrdup(pool, create_name(node, tmpPool)) : node->name;

		apr_hash_set(ht, name, APR_HASH_KEY_STRING, chg); // TODO: Check if name is valid! (maybe use parents names as suffix!)
	}

	if(node->child)
		create_changes_hash(ht, node->child, pool, tmpPool);

	if(node->sibling)
		create_changes_hash(ht, node->sibling, pool, tmpPool);
}

bool SvnLookClient::ChangeInfo(String^ repositoryPath, SvnChangeInfoArgs^ args, EventHandler<SvnChangeInfoEventArgs^>^ changeInfoHandler)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if (changeInfoHandler)
		args->ChangeInfo += changeInfoHandler;

	try
	{
		svn_log_entry_t* entry = svn_log_entry_create(pool.Handle);

		svn_repos_t* repos = nullptr;
		svn_fs_t* fs = nullptr;
		svn_error_t* r;
		apr_hash_t* props = nullptr;

		svn_revnum_t base_rev = 0;
		svn_fs_root_t* root = nullptr;

		if (r = svn_repos_open(
			&repos,
			pool.AllocCanonical(repositoryPath),
			pool.Handle))
		{
			return args->HandleResult(this, r);
		}

		fs = svn_repos_fs(repos);

		if (!String::IsNullOrEmpty(args->Transaction))
		{
			svn_fs_txn_t* txn = nullptr;

			if (r = svn_fs_open_txn(&(txn), fs, pool.AllocString(args->Transaction), pool.Handle))
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

			if (args->Revision < 0L)
			{
				if (r = svn_fs_youngest_rev(&rev, fs, pool.Handle))
					return args->HandleResult(this, r);
			}
			else
				rev = (svn_revnum_t)args->Revision;

			if (r = svn_fs_revision_proplist(&props, fs, rev, pool.Handle))
				return args->HandleResult(this, r);

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
			
			svn_repos_node_t* tree = nullptr;
			{
				AprPool tmpPool(%pool);
				void *edit_baton;

				svn_fs_root_t* base_root;
				if (r = svn_fs_revision_root(&base_root, fs, base_rev, tmpPool.Handle))
					return args->HandleResult(this, r);

				const svn_delta_editor_t *editor;				

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

				create_changes_hash(changes, tree, pool.Handle, tmpPool.Handle);

				entry->changed_paths = changes;
			}
		}

		SvnChangeInfoEventArgs^ e = gcnew SvnChangeInfoEventArgs(entry, base_rev, %pool);
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

bool SvnLookClient::GetChangeInfo(String^ repositoryPath, [Out] SvnChangeInfoEventArgs^% changeInfo)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");

	return GetChangeInfo(repositoryPath, gcnew SvnChangeInfoArgs(), changeInfo);
}

bool SvnLookClient::GetChangeInfo(String^ repositoryPath, SvnChangeInfoArgs^ args, [Out] SvnChangeInfoEventArgs^% changeInfo)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");

	SingleInfoItemReceiver<SvnChangeInfoEventArgs^>^ result = gcnew SingleInfoItemReceiver<SvnChangeInfoEventArgs^>();

	try
	{
		return ChangeInfo(repositoryPath, args, result->Handler);
	}
	finally
	{
		changeInfo = result->Value;
	}
}