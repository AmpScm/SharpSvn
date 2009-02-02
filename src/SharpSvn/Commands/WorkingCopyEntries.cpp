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

bool SvnWorkingCopyClient::ListEntries(String^ directory, SvnWorkingCopyEntriesArgs^ args, EventHandler<SvnWorkingCopyEntryEventArgs^>^ entryHandler)
{
	if (String::IsNullOrEmpty(directory))
		throw gcnew ArgumentNullException("directory");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(directory))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "directory");

	directory = SvnTools::GetNormalizedFullPath(directory);

	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if (entryHandler)
		args->Entry += entryHandler;

	try
	{
		const char* pPath = pool.AllocPath(directory);
		svn_wc_adm_access_t* acc = nullptr;

		svn_error_t* r = svn_wc_adm_open3(&acc, nullptr, pPath, false, 0, CtxHandle->cancel_func, CtxHandle->cancel_baton, pool.Handle);

		if (r)
			return args->HandleResult(this, r);

		apr_hash_t* rslt = nullptr;

		r = svn_wc_entries_read(&rslt, acc, args->RetrieveHidden, pool.Handle);

		if (!r)
		{
			apr_hash_index_t *hi;
			const char* key;
			svn_wc_entry_t *val;

			for (hi = apr_hash_first(pool.Handle, rslt); hi; hi = apr_hash_next(hi))
			{
				apr_hash_this(hi, (const void**)&key, nullptr, (void**)&val);

				SvnWorkingCopyEntryEventArgs^ e = gcnew SvnWorkingCopyEntryEventArgs(directory, key, val);

				args->OnEntry(e);

				e->Detach(false);
			}
		}

		return args->HandleResult(this, r);
	}
	finally
	{
		if (entryHandler)
			args->Entry -= entryHandler;
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