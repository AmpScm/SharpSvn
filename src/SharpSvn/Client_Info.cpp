#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::Info(SvnTarget^ target, EventHandler<SvnInfoEventArgs^>^ infoHandler)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!infoHandler)
		throw gcnew ArgumentNullException("infoHandler");

	Info(target, infoHandler, gcnew SvnInfoArgs());
}

static svn_error_t* svn_info_receiver(void *baton, const char *path, const svn_info_t *info, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnInfoArgs^ args = dynamic_cast<SvnInfoArgs^>(client->CurrentArgs); // C#: _currentArgs as SvnCommitArgs
	if(args)
	{
		SvnInfoEventArgs^ e = gcnew SvnInfoEventArgs(SvnBase::Utf8_PtrToString(path), info);
		try
		{
			args->OnInfo(e);

			if(e->Cancel)
				return svn_error_create(SVN_ERR_CEASE_INVOCATION, NULL, "Info receiver canceled operation");
		}
		catch(Exception^ e)
		{
			AprPool^ thePool = AprPool::Attach(pool, false);
			return svn_error_create(SVN_ERR_CANCELLED, NULL, thePool->AllocString(String::Concat("Info receiver throwed exception: ", e->ToString())));
		}
		finally
		{
			e->Detach(false);
		}
	}

	return nullptr;
}

bool SvnClient::Info(SvnTarget^ target, EventHandler<SvnInfoEventArgs^>^ infoHandler, SvnInfoArgs^ args)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!_pool)
		throw gcnew ObjectDisposedException("SvnClient");

	// We allow a null infoHandler; the args object might just handle it itself

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(_pool);
	_currentArgs = args;
	if(infoHandler)
		args->Info += infoHandler;
	try
	{
		svn_opt_revision_t pegRev = target->Revision->ToSvnRevision();
		svn_opt_revision_t rev = args->Revision->ToSvnRevision();

		svn_error_t* err = svn_client_info(
			pool.AllocString(target->TargetName), 
			&pegRev,
			&rev,
			svn_info_receiver,
			(void*)_clientBatton->Handle,
			IsRecursive(args->Depth),
			CtxHandle,
			pool.Handle);

		return args->HandleResult(err);
	}
	finally
	{
		_currentArgs = nullptr;

		if(infoHandler)
			args->Info -= infoHandler;
	}
}

void SvnClient::GetInfo(SvnTarget^ target, [Out] SvnInfoEventArgs^% info)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	InfoItemList<SvnInfoEventArgs^>^ results = gcnew InfoItemList<SvnInfoEventArgs^>();

	try
	{
		Info(target, results->Handler);
	}
	finally
	{
		if(results->Count > 0)
			info = results[0];
		else
			info = nullptr;
	}
}

bool SvnClient::GetInfo(SvnTarget^ target, SvnInfoArgs^ args, [Out] IList<SvnInfoEventArgs^>^ info)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	InfoItemList<SvnInfoEventArgs^>^ results = gcnew InfoItemList<SvnInfoEventArgs^>();

	try
	{
		return Info(target, results->Handler, args);	
	}
	finally
	{
		info = safe_cast<IList<SvnInfoEventArgs^>^>(results);
	}
}