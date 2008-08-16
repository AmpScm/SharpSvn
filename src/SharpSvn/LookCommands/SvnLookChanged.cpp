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

	return Changed(repositoryPath, gcnew SvnChangedArgs(), changedHandler);
}

bool SvnLookClient::Changed(String^ repositoryPath, SvnChangedArgs^ args, EventHandler<SvnChangedEventArgs^>^ changedHandler)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if (changedHandler)
		args->Changed += changedHandler;

	try
	{
	svn_error_t* r;
		svn_repos_t* repos;
		if (r = svn_repos_open(&repos, pool.AllocPath(repositoryPath), pool.Handle))
			return args->HandleResult(this, r);

		svn_fs_t* fs = svn_repos_fs(repos);
		svn_revnum_t base_rev;
		svn_fs_txn_t* txn = NULL;
		svn_fs_root_t* root;

		// Figure out whether they are asking for a specific transaction or
		// revision
		if (!String::IsNullOrEmpty(args->Transaction))
		{
			if (r = svn_fs_open_txn(&(txn), fs, pool.AllocString(args->Transaction), pool.Handle))
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
		r = ProcessTree(tree, repositoryPath, String::Empty, args);

		return args->HandleResult(this, r);
	}
	finally
	{
		if (changedHandler)
			args->Changed -= changedHandler;
	}
}

svn_error_t* SvnLookClient::ProcessTree(svn_repos_node_t *node, String^ path, String^ relativePath, SvnChangedArgs^ args)
{
	if (node)
	{
		SvnChangedEventArgs^ e = gcnew SvnChangedEventArgs(
			relativePath + SvnBase::Utf8_PtrToString(node->name),
			(SvnChangeAction)node->action,
			SvnBase::Utf8_PtrToString(node->copyfrom_path),
			node->copyfrom_rev,
			(SvnNodeKind)node->kind,
			node->prop_mod ? true : false,
			node->text_mod ? true : false
		);

		if (e->Kind == SvnNodeKind::Directory && !String::IsNullOrEmpty(e->Name))
		{
			relativePath = e->Name + "/";
		}

		// Fire off the actual object
		try
		{
			args->OnChanged(e);

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

		node = node->child;
		if (node)
		{
			AprPool subpool(%_pool);
			String^ fullpath = SvnPathJoin(path, SvnBase::Utf8_PtrToString(node->name));
			svn_error_t* r = ProcessTree(node, fullpath, relativePath, args);

			if (r)
				return r;

			while (node->sibling)
			{
				subpool.Clear();
				node = node->sibling;
				fullpath = SvnPathJoin(path, SvnBase::Utf8_PtrToString(node->name));
				r = ProcessTree(node, fullpath, relativePath, args);
				if (r)
					return r;
			}
		}
	}

	return nullptr;
}

String^ SvnLookClient::PreparePath(String^ path)
{
	if (path == nullptr)
	{
		throw gcnew ArgumentException("Local path not specified");
	}
	path = path->Trim();
	path = path->Replace('\\', '/');
	while (path->EndsWith("/"))
	{
		path = path->Substring(0, path->Length - 1);
	}

	return path;
}

String^ SvnLookClient::SvnPathJoin(String^ base, String^ component)
{
	if (String::IsNullOrEmpty(base) && String::IsNullOrEmpty(component))
	{
		throw gcnew ArgumentNullException("base", "Cannot combine two blank paths");
	}
	if (String::IsNullOrEmpty(component))
	{
		return base;
	}
	else if (String::IsNullOrEmpty(base))
	{
		return component;
	}

	base = PreparePath(base);
	component = PreparePath(component);

	if (component[0] == '/')
	{
		return component;
	}

	if (base->Length > 0 && base[base->Length - 1] == '/')
	{
		base = base->Substring(0, base->Length - 1);
	}

	return base + "/" + component;
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
