#include "stdafx.h"
#include "SvnAll.h"
#include "LookArgs/SvnLookListArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace System::Collections::Generic;

bool SvnLookClient::List(SvnLookOrigin^ lookOrigin, String^ path, EventHandler<SvnLookListEventArgs^>^ listHandler)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (!path)
		throw gcnew ArgumentNullException("path");
	else if (!IsNotUri(path) && path->Length)
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if (!listHandler)
		throw gcnew ArgumentNullException("listHandler");

	return List(lookOrigin, path, gcnew SvnLookListArgs(), listHandler);
}

static svn_error_t*
ListEntries(SvnLookListArgs^ args, svn_fs_root_t *root, const char *path, bool recurse, AprPool^ pool)
{
	AprPool subpool(pool);
	apr_hash_t *entries;

	SVN_ERR(svn_fs_dir_entries(&entries, root, path, subpool.Handle));

	for (apr_hash_index_t *hi = apr_hash_first(subpool.Handle, entries); hi; hi = apr_hash_next(hi))
	{
		svn_fs_dirent_t* item;
		apr_hash_this(hi, nullptr, nullptr, (void**)&item);

		switch (item->kind)
		{
		case svn_node_file:
			if (args->Depth < SvnDepth::Files)
				continue;
			break;
		case svn_node_dir:
			if (args->Depth < SvnDepth::Children)
				continue;
			break;
		}

		SvnLookListEventArgs^ ee = gcnew SvnLookListEventArgs(path, item, pool);
		try
		{
			args->InvokeList(ee);
		}
		finally
		{
			ee->Detach(false);
		}

		if (ee->Cancel)
			return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "List receiver canceled operation");

		if (recurse && item->kind == svn_node_dir)
		{
			SVN_ERR(ListEntries(args, root, svn_path_join(path, item->name, subpool.Handle), true, %subpool));
		}		
	}	

	return SVN_NO_ERROR;
}

bool SvnLookClient::List(SvnLookOrigin^ lookOrigin, String^ path, SvnLookListArgs^ args, EventHandler<SvnLookListEventArgs^>^ listHandler)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (!path)
		throw gcnew ArgumentNullException("path");
	else if (!IsNotUri(path) && path->Length)
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	// We allow a null listHandler; the args object might just handle it itself

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if (listHandler)
		args->List += listHandler;
	try
	{
		svn_fs_root_t* root = nullptr;
		svn_error_t* r;

		if (r = open_origin(lookOrigin, &root, nullptr, nullptr, %pool))
			return args->HandleResult(this, r);

		r = ListEntries(args, root, pool.AllocPath(path), args->Depth == SvnDepth::Infinity, %pool);
		return args->HandleResult(this, r);
	}
	finally
	{
		if (listHandler)
			args->List -= listHandler;
	}
}

bool SvnLookClient::GetList(SvnLookOrigin^ lookOrigin, String^ path, [Out] Collection<SvnLookListEventArgs^>^% list)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (!path)
		throw gcnew ArgumentNullException("path");
	else if (!IsNotUri(path) && path->Length)
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	InfoItemCollection<SvnLookListEventArgs^>^ results = gcnew InfoItemCollection<SvnLookListEventArgs^>();

	try
	{
		return List(lookOrigin, path, gcnew SvnLookListArgs(), results->Handler);
	}
	finally
	{
		list = results;
	}
}


bool SvnLookClient::GetList(SvnLookOrigin^ lookOrigin, String^ path, SvnLookListArgs^ args, [Out] Collection<SvnLookListEventArgs^>^% list)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (!path)
		throw gcnew ArgumentNullException("path");
	else if (!IsNotUri(path) && path->Length)
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnLookListEventArgs^>^ results = gcnew InfoItemCollection<SvnLookListEventArgs^>();

	try
	{
		return List(lookOrigin, path, args, results->Handler);
	}
	finally
	{
		list = results;
	}
}
