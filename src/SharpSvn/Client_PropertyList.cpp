// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;


[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetPropertyList(SharpSvn.SvnTarget,SharpSvn.SvnPropertyListArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnPropertyListEventArgs>&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetPropertyList(SharpSvn.SvnTarget,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnPropertyListEventArgs>&)", MessageId="1#")];
bool SvnClient::PropertyList(SvnTarget^ target, EventHandler<SvnPropertyListEventArgs^>^ listHandler)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!listHandler)
		throw gcnew ArgumentNullException("listHandler");

	return PropertyList(target, gcnew SvnPropertyListArgs(), listHandler);
}

static svn_error_t *svnclient_property_list_handler(void *baton, const char *path, apr_hash_t *prop_hash, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnPropertyListArgs^ args = dynamic_cast<SvnPropertyListArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs
	AprPool aprPool(pool, false);
	if(!args)
		return nullptr;

	SvnPropertyListEventArgs^ e = gcnew SvnPropertyListEventArgs(path, prop_hash, %aprPool);
	try
	{
		args->OnPropertyList(e);

		if(e->Cancel)
			return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "List receiver canceled operation");
		else
			return nullptr;
	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("Property list receiver", e);
	}
	finally
	{
		e->Detach(false);
	}
}

bool SvnClient::PropertyList(SvnTarget^ target, SvnPropertyListArgs^ args, EventHandler<SvnPropertyListEventArgs^>^ listHandler)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	// We allow a null listHandler; the args object might just handle it itself

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if(listHandler)
		args->PropertyList += listHandler;
	try
	{
		svn_opt_revision_t pegrev = target->Revision->ToSvnRevision();
		svn_opt_revision_t rev = args->Revision->ToSvnRevision();

		svn_error_t* r = svn_client_proplist3(
			pool.AllocString(target->TargetName),
			&pegrev,
			&rev,
			(svn_depth_t)args->Depth,
			CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
			svnclient_property_list_handler,
			(void*)_clientBatton->Handle,			
			CtxHandle,
			pool.Handle);

		return args->HandleResult(this, r);
	}
	finally
	{
		if(listHandler)
			args->PropertyList -= listHandler;
	}
}

bool SvnClient::GetPropertyList(SvnTarget^ target, [Out] Collection<SvnPropertyListEventArgs^>^% list)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	InfoItemCollection<SvnPropertyListEventArgs^>^ results = gcnew InfoItemCollection<SvnPropertyListEventArgs^>();

	try
	{
		return PropertyList(target, gcnew SvnPropertyListArgs(), results->Handler);
	}
	finally
	{
		list = results;
	}
}

bool SvnClient::GetPropertyList(SvnTarget^ target, SvnPropertyListArgs^ args, [Out] Collection<SvnPropertyListEventArgs^>^% list)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnPropertyListEventArgs^>^ results = gcnew InfoItemCollection<SvnPropertyListEventArgs^>();

	try
	{
		return PropertyList(target, args, results->Handler);
	}
	finally
	{
		list = results;
	}
}
