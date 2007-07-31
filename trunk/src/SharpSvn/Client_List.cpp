#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::List(SvnTarget^ target, EventHandler<SvnListEventArgs^>^ listHandler)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!listHandler)
		throw gcnew ArgumentNullException("listHandler");
	
	List(target, listHandler, gcnew SvnListArgs());
}

static svn_error_t *svnclient_list_handler(void *baton, const char *path, const svn_dirent_t *dirent, const svn_lock_t *lock, const char *abs_path, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnListArgs^ args = dynamic_cast<SvnListArgs^>(client->CurrentArgs); // C#: _currentArgs as SvnCommitArgs
	if(args)
	{
		SvnListEventArgs^ e = gcnew SvnListEventArgs(path, dirent, lock, abs_path);
		try
		{
			args->OnList(e);

			if(e->Cancel)
				return svn_error_create (SVN_ERR_CANCELLED, NULL, "List receiver canceled operation");
		}
		catch(Exception^ e)
		{
			AprPool^ thePool = AprPool::Attach(pool, false);
			return svn_error_create(SVN_ERR_CANCELLED, NULL, thePool->AllocString(String::Concat("List receiver throwed exception: ", e->ToString())));
		}
		finally
		{
			e->Detach(false);
		}
	}

	return nullptr;
}

bool SvnClient::List(SvnTarget^ target, EventHandler<SvnListEventArgs^>^ listHandler, SvnListArgs^ args)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!_pool)
		throw gcnew ObjectDisposedException("SvnClient");

	// We allow a null listHandler; the args object might just handle it itself

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(_pool);
	_currentArgs = args;
	if(listHandler)
		args->List += listHandler;
	try
	{
		svn_opt_revision_t pegrev = target->Revision->ToSvnRevision();
		svn_opt_revision_t rev = args->Revision->ToSvnRevision();

		svn_error_t* err = svn_client_list2(
			pool.AllocString(target->ToString()), 
			&pegrev,
			&rev, 
			(svn_depth_t)args->Depth,
			(apr_uint32_t)args->EntryItems,
			args->FetchLocks,
			svnclient_list_handler,
			(void*)_clientBatton->Handle,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(err);
	}
	finally
	{
		_currentArgs = nullptr;

		if(listHandler)
			args->List -= listHandler;
	}
}

void SvnClient::GetList(SvnTarget^ target, [Out] IList<SvnListEventArgs^>^% list)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	InfoItemList<SvnListEventArgs^>^ results = gcnew InfoItemList<SvnListEventArgs^>();

	try
	{
		List(target, results->Handler, gcnew SvnListArgs());	
	}
	finally
	{
		list = safe_cast<IList<SvnListEventArgs^>^>(results);
	}
}


bool SvnClient::GetList(SvnTarget^ target, SvnListArgs^ args, [Out] IList<SvnListEventArgs^>^% list)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	InfoItemList<SvnListEventArgs^>^ results = gcnew InfoItemList<SvnListEventArgs^>();

	try
	{
		return List(target, results->Handler, args);	
	}
	finally
	{
		list = safe_cast<IList<SvnListEventArgs^>^>(results);
	}
}