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
#include "Args/MergesMerged.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;


[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetMergesMerged(SharpSvn.SvnTarget,SharpSvn.SvnTarget,SharpSvn.SvnMergesMergedArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnMergesMergedEventArgs>&)", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetMergesMerged(SharpSvn.SvnTarget,SharpSvn.SvnTarget,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnMergesMergedEventArgs>&)", MessageId="2#")];

bool SvnClient::ListMergesMerged(SvnTarget ^target, SvnTarget^ source, EventHandler<SvnMergesMergedEventArgs^>^ handler)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!source)
		throw gcnew ArgumentNullException("source");
	else if (!handler)
		throw gcnew ArgumentNullException("handler");

	return ListMergesMerged(target, source, gcnew SvnMergesMergedArgs(), handler);
}

static svn_error_t *svnclient_eligible_log_handler(void *baton, svn_log_entry_t *log_entry, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnMergesMergedArgs^ args = dynamic_cast<SvnMergesMergedArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnLogArgs
	AprPool aprPool(pool, false);
	if (!args)
		return nullptr;

	SvnMergesMergedEventArgs^ e = gcnew SvnMergesMergedEventArgs(log_entry, args->_sourceTarget, %aprPool);

	try
	{
		args->OnMergesMerged(e);

		if (e->Cancel)
			return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Log receiver canceled operation");
		else
			return nullptr;
	}
	catch(Exception^ ex)
	{
		return SvnException::CreateExceptionSvnError("Log receiver", ex);
	}
	finally
	{
		e->Detach(false);
	}
}


bool SvnClient::ListMergesMerged(SvnTarget ^target, SvnTarget^ source, SvnMergesMergedArgs^ args, EventHandler<SvnMergesMergedEventArgs^>^ handler)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!source)
		throw gcnew ArgumentNullException("source");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	if (handler)
		args->List += handler;
	try
	{
		apr_array_header_t* retrieveProperties;

		if (args->RetrievePropertiesUsed)
			retrieveProperties = args->RetrieveProperties->Count ? AllocArray(args->RetrieveProperties, %pool) : nullptr;
		else
			retrieveProperties = svn_compat_log_revprops_in(pool.Handle);

		args->_sourceTarget = source;

		svn_error_t* r = svn_client_mergeinfo_log(
			TRUE,
			target->AllocAsString(%pool),
			target->Revision->AllocSvnRevision(%pool),
			source->AllocAsString(%pool),
			source->Revision->AllocSvnRevision(%pool),
			svnclient_eligible_log_handler,
			(void*)_clientBaton->Handle,
			args->RetrieveChangedPaths,
			(svn_depth_t)args->Depth,
			retrieveProperties,
			CtxHandle,
			pool.Handle);

			return args->HandleResult(this, r, target);
	}
	finally
	{
		if (handler)
			args->List -= handler;

		args->_sourceTarget = nullptr;
	}
}

bool SvnClient::GetMergesMerged(SvnTarget ^target, SvnTarget^ source, [Out] Collection<SvnMergesMergedEventArgs^>^% list)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!source)
		throw gcnew ArgumentNullException("source");

	return GetMergesMerged(target, source, gcnew SvnMergesMergedArgs(), list);
}

bool SvnClient::GetMergesMerged(SvnTarget ^target, SvnTarget^ source, SvnMergesMergedArgs^ args, [Out] Collection<SvnMergesMergedEventArgs^>^% list)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!source)
		throw gcnew ArgumentNullException("source");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnMergesMergedEventArgs^>^ results = gcnew InfoItemCollection<SvnMergesMergedEventArgs^>();

	try
	{
		return ListMergesMerged(target, source, args, results->Handler);
	}
	finally
	{
		list = results;
	}
}