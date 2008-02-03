// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.GetChangelist(System.String,System.String,SharpSvn.SvnListChangelistArgs,System.Collections.Generic.IList`1<System.String>&):System.Boolean")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetChangelist(System.String,SharpSvn.SvnListChangelistArgs,System.Collections.Generic.IList`1<SharpSvn.SvnListChangelistEventArgs>&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetChangelist(System.String,System.Collections.Generic.IList`1<SharpSvn.SvnListChangelistEventArgs>&):System.Boolean", MessageId="1#")];

bool SvnClient::ListChangelist(String^ rootPath, EventHandler<SvnListChangelistEventArgs^>^ changelistHandler)
{
	if(String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");
	else if(!changelistHandler)
		throw gcnew ArgumentNullException("changelistHandler");

	return ListChangelist(rootPath, gcnew SvnListChangelistArgs(), changelistHandler);
}

static svn_error_t *svnclient_changelist_handler(void *baton, const char *path, const char *changelist, apr_pool_t *pool)
{
	UNUSED_ALWAYS(pool);
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnListChangelistArgs^ args = dynamic_cast<SvnListChangelistArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs
	if(args)
	{
		SvnListChangelistEventArgs^ e = gcnew SvnListChangelistEventArgs(path, changelist);
		try
		{
			args->OnListChangelist(e);

			if(e->Cancel)
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


bool SvnClient::ListChangelist(String^ rootPath, SvnListChangelistArgs^ args, EventHandler<SvnListChangelistEventArgs^>^ changelistHandler)
{
	if(String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if(changelistHandler)
		args->ListChangelist += changelistHandler;
	try
	{
		svn_error_t* r = svn_client_get_changelists(
			pool.AllocPath(rootPath),
			CreateChangelistsList(args->Changelists, %pool), // Intersect Changelists
			(svn_depth_t)args->Depth,
			svnclient_changelist_handler,
			(void*)_clientBatton->Handle,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(this, r);
	}
	finally
	{
		if(changelistHandler)
			args->ListChangelist -= changelistHandler;
	}
}

bool SvnClient::GetChangelist(String^ rootPath, [Out]IList<SvnListChangelistEventArgs^>^% list)
{
	if(String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");

	return GetChangelist(rootPath, gcnew SvnListChangelistArgs(), list);
}

bool SvnClient::GetChangelist(String^ rootPath, SvnListChangelistArgs^ args, [Out]IList<SvnListChangelistEventArgs^>^% list)
{
	if(String::IsNullOrEmpty(rootPath))
		throw gcnew ArgumentNullException("rootPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	IList<SvnListChangelistEventArgs^>^ who = nullptr;
	list = nullptr;

	InfoItemCollection<SvnListChangelistEventArgs^>^ results = gcnew InfoItemCollection<SvnListChangelistEventArgs^>();

	try
	{
		return ListChangelist(rootPath, args, results->Handler);
	}
	finally
	{
		list = safe_cast<IList<SvnListChangelistEventArgs^>^>(who);
	}
}
