// $Id$
//
// Copyright 2007-2009 The SharpSvn Project
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
#include "Args/MergesEligible.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetEligableMergeInfo(SharpSvn.SvnTarget,System.Uri,SharpSvn.SvnGetEligableMergeInfoArgs,SharpSvn.SvnEligableMergeInfo&)", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetEligableMergeInfo(SharpSvn.SvnTarget,System.Uri,SharpSvn.SvnEligableMergeInfo&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.#GetEligableMergeInfo(SharpSvn.SvnTarget,System.Uri,SharpSvn.SvnGetEligableMergeInfoArgs,SharpSvn.SvnEligableMergeInfo&)")];

bool SvnClient::ListMergesEligible(SvnTarget ^target, SvnTarget^ source, EventHandler<SvnMergesEligibleEventArgs^>^ logHandler)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!source)
		throw gcnew ArgumentNullException("source");
	else if (!logHandler)
		throw gcnew ArgumentNullException("logHandler");

	return ListMergesEligible(target, source, gcnew SvnMergesEligibleArgs(), logHandler);
}

static svn_error_t *svnclient_eligible_log_handler(void *baton, svn_log_entry_t *log_entry, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnMergesEligibleArgs^ args = dynamic_cast<SvnMergesEligibleArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnLogArgs
	AprPool aprPool(pool, false);
	if (!args)
		return nullptr;

	SvnMergesEligibleEventArgs^ e = gcnew SvnMergesEligibleEventArgs(log_entry, args->_sourceTarget, %aprPool);

	try
	{
		args->OnList(e);

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


bool SvnClient::ListMergesEligible(SvnTarget ^target, SvnTarget^ source, SvnMergesEligibleArgs^ args, EventHandler<SvnMergesEligibleEventArgs^>^ handler)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!source)
		throw gcnew ArgumentNullException("source");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

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

		svn_error_t* r = svn_client_mergeinfo_log_eligible(
			pool.AllocString(target->SvnTargetName),
			target->Revision->AllocSvnRevision(%pool),
			pool.AllocString(source->SvnTargetName),
			source->Revision->AllocSvnRevision(%pool),
			svnclient_eligible_log_handler,
			(void*)_clientBatton->Handle,
			args->RetrieveChangedPaths,
			retrieveProperties,
			CtxHandle,
			pool.Handle);

			return args->HandleResult(this, r);
	}
	finally
	{
		if (handler)
			args->List -= handler;
		args->_sourceTarget = nullptr;
	}
}

bool SvnClient::GetMergesEligible(SvnTarget ^target, SvnTarget^ source, [Out] Collection<SvnMergesEligibleEventArgs^>^% list)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!source)
		throw gcnew ArgumentNullException("source");

	return GetMergesEligible(target, source, gcnew SvnMergesEligibleArgs(), list);
}

bool SvnClient::GetMergesEligible(SvnTarget ^target, SvnTarget^ source, SvnMergesEligibleArgs^ args, [Out] Collection<SvnMergesEligibleEventArgs^>^% list)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!source)
		throw gcnew ArgumentNullException("source");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnMergesEligibleEventArgs^>^ results = gcnew InfoItemCollection<SvnMergesEligibleEventArgs^>();

	try
	{
		return ListMergesEligible(target, source, args, results->Handler);
	}
	finally
	{
		list = results;
	}
}