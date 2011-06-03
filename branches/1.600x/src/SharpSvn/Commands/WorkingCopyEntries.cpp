// $Id$
//
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
#include "Args/WorkingCopyEntries.h"
#include "EventArgs/SvnWorkingCopyEntryEventArgs.h"

#include "UnmanagedStructs.h"

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnWorkingCopyClient.#GetEntries(System.String,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnWorkingCopyEntryEventArgs>&)", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnWorkingCopyClient.#GetEntries(System.String,SharpSvn.SvnWorkingCopyEntriesArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnWorkingCopyEntryEventArgs>&)", MessageId="2#")];

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnWorkingCopyClient::ListEntries(String^ directory, EventHandler<SvnWorkingCopyEntryEventArgs^>^ entryHandler)
{
	if (String::IsNullOrEmpty(directory))
		throw gcnew ArgumentNullException("directory");
	else if (!entryHandler)
		throw gcnew ArgumentNullException("entryHandler");
	else if (!IsNotUri(directory))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "directory");

	return ListEntries(directory, gcnew SvnWorkingCopyEntriesArgs(), entryHandler);
}

/** An @a entry was found at @a path. */
static svn_error_t *
sharpsvn_found_entry(const char *path, const svn_wc_entry_t *entry, void *walk_baton, apr_pool_t *pool)
{
	SvnWorkingCopyClient^ client = AprBaton<SvnWorkingCopyClient^>::Get((IntPtr)walk_baton);

	AprPool thePool(pool, false);

	SvnWorkingCopyEntriesArgs^ args = dynamic_cast<SvnWorkingCopyEntriesArgs^>(client->CurrentCommandArgs);
	if (!args)
		return nullptr;

	if (entry->kind == svn_node_dir &&
		args->Depth > SvnDepth::Unknown &&
		svn_path_is_empty(entry->name))
	{
		/* Skip directories in the parent to get some sane behavior for future versions */
		return nullptr;
	}

	SvnWorkingCopyEntryEventArgs^ e = gcnew SvnWorkingCopyEntryEventArgs(args->_dir, path, entry, %thePool);
	try
	{
		args->OnEntry(e);

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

	return SVN_NO_ERROR;
}

static svn_error_t *
sharpsvn_handle_error(const char *path, svn_error_t *err, void *walk_baton, apr_pool_t *pool)
{
	//SvnWorkingCopyClient^ client = AprBaton<SvnWorkingCopyClient^>::Get((IntPtr)walk_baton);
	UNUSED_ALWAYS(path);
	UNUSED_ALWAYS(err);
	UNUSED_ALWAYS(walk_baton);
	UNUSED_ALWAYS(pool);

	svn_error_clear(err);

	return SVN_NO_ERROR; /* Continue walking */
}

bool SvnWorkingCopyClient::ListEntries(String^ directory, SvnWorkingCopyEntriesArgs^ args, EventHandler<SvnWorkingCopyEntryEventArgs^>^ entryHandler)
{
	if (String::IsNullOrEmpty(directory))
		throw gcnew ArgumentNullException("directory");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(directory))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "directory");

	directory = SvnTools::GetNormalizedFullPath(directory);

	static const svn_wc_entry_callbacks2_t walk_callbacks =
	{ sharpsvn_found_entry, sharpsvn_handle_error };

	ArgsStore store(this, args);
	AprPool pool(%_pool);

	args->_dir = directory;

	if (entryHandler)
		args->Entry += entryHandler;

	try
	{
		const char* pPath = pool.AllocPath(directory);
		svn_wc_adm_access_t* adm_access = nullptr;

		svn_error_t* r = svn_wc_adm_open3(
			&adm_access,
			nullptr,
			pPath,
			false,
			args->Depth >= SvnDepth::Children ? -1 : 0,
			CtxHandle->cancel_func,
			CtxHandle->cancel_baton,
			pool.Handle);

		if (r)
			return args->HandleResult(this, r, directory);

		if (args->Depth != SvnDepth::Unknown)
		{
			r = svn_wc_walk_entries3(
				pPath,
				adm_access,
				&walk_callbacks,
				(void*)_clientBaton->Handle,
				(svn_depth_t)args->Depth,
				args->RetrieveHidden,
				nullptr,
				nullptr,
				pool.Handle);
		}
		else
		{
			apr_hash_t* rslt = nullptr;

			r = svn_wc_entries_read(&rslt, adm_access, args->RetrieveHidden, pool.Handle);

			if (!r)
			{
				apr_hash_index_t *hi;
				const char* key;
				svn_wc_entry_t *val;

				for (hi = apr_hash_first(pool.Handle, rslt); hi; hi = apr_hash_next(hi))
				{
					apr_hash_this(hi, (const void**)&key, nullptr, (void**)&val);

					SvnWorkingCopyEntryEventArgs^ e = gcnew SvnWorkingCopyEntryEventArgs(directory, key, val, %pool);

					args->OnEntry(e);

					e->Detach(false);

					if(e->Cancel)
					{
						r = svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "List receiver canceled operation");
						break;
					}
				}
			}

		}

		return args->HandleResult(this, r, directory);
	}
	finally
	{
		if (entryHandler)
			args->Entry -= entryHandler;

		args->_dir = nullptr;
	}
}

bool SvnWorkingCopyClient::GetEntries(String^ directory, [Out] Collection<SvnWorkingCopyEntryEventArgs^>^% list)
{
	if (!directory)
		throw gcnew ArgumentNullException("directory");

	InfoItemCollection<SvnWorkingCopyEntryEventArgs^>^ results = gcnew InfoItemCollection<SvnWorkingCopyEntryEventArgs^>();

	try
	{
		return ListEntries(directory, gcnew SvnWorkingCopyEntriesArgs(), results->Handler);
	}
	finally
	{
		list = results;
	}
}

bool SvnWorkingCopyClient::GetEntries(String^ directory, SvnWorkingCopyEntriesArgs^ args,[Out] Collection<SvnWorkingCopyEntryEventArgs^>^% list)
{
	if (!directory)
		throw gcnew ArgumentNullException("directory");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnWorkingCopyEntryEventArgs^>^ results = gcnew InfoItemCollection<SvnWorkingCopyEntryEventArgs^>();

	try
	{
		return ListEntries(directory, args, results->Handler);
	}
	finally
	{
		list = results;
	}
}
