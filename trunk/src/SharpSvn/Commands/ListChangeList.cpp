// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/ListChangeList.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.GetChangeList(System.String,System.String,SharpSvn.SvnListChangeListArgs,System.Collections.ObjectModel.Collection`1<System.String>&):System.Boolean")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetChangeList(System.String,SharpSvn.SvnListChangeListArgs,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnListChangeListEventArgs>&)", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.#GetChangeList(System.String,System.Collections.ObjectModel.Collection`1<SharpSvn.SvnListChangeListEventArgs>&)", MessageId="1#")];

bool SvnClient::ListChangeList(String^ rootPath, EventHandler<SvnListChangeListEventArgs^>^ changeListHandler)
{
	if (String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");
	else if (!changeListHandler)
		throw gcnew ArgumentNullException("changeListHandler");

	return ListChangeList(rootPath, gcnew SvnListChangeListArgs(), changeListHandler);
}

static svn_error_t *svnclient_changelist_handler(void *baton, const char *path, const char *changelist, apr_pool_t *pool)
{
	UNUSED_ALWAYS(pool);
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnListChangeListArgs^ args = dynamic_cast<SvnListChangeListArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs
	if (args)
	{
		SvnListChangeListEventArgs^ e = gcnew SvnListChangeListEventArgs(path, changelist);
		try
		{
			args->OnListChangeList(e);

			if (e->Cancel)
				return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "List receiver canceled operation");
		}
		catch(Exception^ e)
		{
			AprPool^ thePool = gcnew AprPool(); // We have no error pool handler; just create a GC-able one; it is just for copying into the error

			return svn_error_create(SVN_ERR_CANCELLED, nullptr, thePool->AllocString(String::Concat("List receiver throwed exception: ", e->ToString())));
		}
		finally
		{
			e->Detach(false);
		}
	}

	return nullptr;
}


bool SvnClient::ListChangeList(String^ rootPath, SvnListChangeListArgs^ args, EventHandler<SvnListChangeListEventArgs^>^ changeListHandler)
{
	if (String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if (changeListHandler)
		args->ListChangeList += changeListHandler;
	try
	{
		svn_error_t* r = svn_client_get_changelists(
			pool.AllocPath(rootPath),
			CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
			(svn_depth_t)args->Depth,
			svnclient_changelist_handler,
			(void*)_clientBatton->Handle,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(this, r);
	}
	finally
	{
		if (changeListHandler)
			args->ListChangeList -= changeListHandler;
	}
}

bool SvnClient::GetChangeList(String^ rootPath, [Out]Collection<SvnListChangeListEventArgs^>^% list)
{
	if (String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");

	return GetChangeList(rootPath, gcnew SvnListChangeListArgs(), list);
}

bool SvnClient::GetChangeList(String^ rootPath, SvnListChangeListArgs^ args, [Out]Collection<SvnListChangeListEventArgs^>^% list)
{
	if (String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	list = nullptr;

	InfoItemCollection<SvnListChangeListEventArgs^>^ results = gcnew InfoItemCollection<SvnListChangeListEventArgs^>();

	try
	{
		return ListChangeList(rootPath, args, results->Handler);
	}
	finally
	{
		list = results;
	}
}
