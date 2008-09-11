// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "LookArgs/SvnChangedArgs.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnLookClient::Changed(String^ repositoryPath, EventHandler<SvnChangedEventArgs^>^ changedHandler)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if(!changedHandler)
		throw gcnew ArgumentNullException("changedHandler");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");

	return Changed(repositoryPath, gcnew SvnChangedArgs(), changedHandler);
}

bool SvnLookClient::Changed(String^ repositoryPath, SvnChangedArgs^ args, EventHandler<SvnChangedEventArgs^>^ changedHandler)
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

	if (changedHandler)
		args->Changed += changedHandler;

	try
	{
		svn_error_t* r;
		svn_repos_t* repos;
		if (r = svn_repos_open(&repos, pool.AllocCanonical(repositoryPath), pool.Handle))
			return args->HandleResult(this, r);

		svn_fs_t* fs = svn_repos_fs(repos);
		svn_revnum_t base_rev;
		svn_fs_txn_t* txn = NULL;
		svn_fs_root_t* root;

		// Figure out whether they are asking for a specific transaction or
		// revision
		if (!String::IsNullOrEmpty(args->Transaction))
		{
			if (r = svn_fs_open_txn(&txn, fs, pool.AllocString(args->Transaction), pool.Handle))
				return args->HandleResult(this, r);

			base_rev = svn_fs_txn_base_revision(txn);

			if (r = svn_fs_txn_root(&root, txn, pool.Handle))
				return args->HandleResult(this, r);
		}
		else
		{
			svn_revnum_t rev;

			if (!SVN_IS_VALID_REVNUM(args->Revision->Revision) ||
				(args->Revision->Revision == 0))
			{
				if (r = svn_fs_youngest_rev(&rev, fs, pool.Handle))
					return args->HandleResult(this, r);
			}
			else
			{
				rev = (svn_revnum_t)args->Revision->Revision;
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
		catch (Exception^ e)
		{
			return SvnException::CreateExceptionSvnError("Changed receiver", e);
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

bool SvnLookClient::GetChanged(String^ repositoryPath, [Out] Collection<SvnChangedEventArgs^>^% changedItems)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");

	InfoItemCollection<SvnChangedEventArgs^>^ results = gcnew InfoItemCollection<SvnChangedEventArgs^>();

	try
	{
		return Changed(repositoryPath, gcnew SvnChangedArgs(), results->Handler);
	}
	finally
	{
		changedItems = results;
	}
}

bool SvnLookClient::GetChanged(String^ repositoryPath, SvnChangedArgs^ args, [Out] Collection<SvnChangedEventArgs^>^% changedItems)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnChangedEventArgs^>^ results = gcnew InfoItemCollection<SvnChangedEventArgs^>();

	try
	{
		return Changed(repositoryPath, args, results->Handler);
	}
	finally
	{
		changedItems = results;
	}
}
