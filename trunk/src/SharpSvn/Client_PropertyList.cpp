#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::PropertyList(SvnTarget^ target, EventHandler<SvnPropertyListEventArgs^>^ listHandler)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!listHandler)
		throw gcnew ArgumentNullException("listHandler");
	
	PropertyList(target, listHandler, gcnew SvnPropertyListArgs());
}

static svn_error_t *svnclient_property_list_handler(void *baton, const char *path, apr_hash_t *prop_hash, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnPropertyListArgs^ args = dynamic_cast<SvnPropertyListArgs^>(client->CurrentArgs); // C#: _currentArgs as SvnCommitArgs
	AprPool^ aprPool = AprPool::Attach(pool, false);
	if(args)
	{
		SvnPropertyListEventArgs^ e = gcnew SvnPropertyListEventArgs(path, prop_hash, aprPool);
		try
		{
			args->OnPropertyList(e);

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

bool SvnClient::PropertyList(SvnTarget^ target, EventHandler<SvnPropertyListEventArgs^>^ listHandler, SvnPropertyListArgs^ args)
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
		args->PropertyList += listHandler;
	try
	{
		svn_opt_revision_t pegrev = target->Revision->ToSvnRevision();
		svn_opt_revision_t rev = args->Revision->ToSvnRevision();

		svn_error_t* err = svn_client_proplist3(
			pool.AllocString(target->ToString()), 
			&pegrev,
			&rev, 
			(svn_depth_t)args->Depth,
			svnclient_property_list_handler,
			(void*)_clientBatton->Handle,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(err);
	}
	finally
	{
		_currentArgs = nullptr;

		if(listHandler)
			args->PropertyList -= listHandler;
	}
}

void SvnClient::GetPropertyList(SvnTarget^ target, [Out] IList<SvnPropertyListEventArgs^>^% list)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	InfoItemList<SvnPropertyListEventArgs^>^ results = gcnew InfoItemList<SvnPropertyListEventArgs^>();

	try
	{
		PropertyList(target, results->Handler, gcnew SvnPropertyListArgs());	
	}
	finally
	{
		list = safe_cast<IList<SvnPropertyListEventArgs^>^>(results);
	}
}

bool SvnClient::GetPropertyList(SvnTarget^ target, SvnPropertyListArgs^ args, [Out] IList<SvnPropertyListEventArgs^>^% list)
{
	return false;
}
