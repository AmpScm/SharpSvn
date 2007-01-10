#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::Status(SvnPathTarget^ path, EventHandler<SvnStatusEventArgs^>^ statusHandler)
{
	if(!path)
		throw gcnew ArgumentNullException("path");
	else if(!statusHandler)
		throw gcnew ArgumentNullException("statusHandler");
	
	Status(path, statusHandler, gcnew SvnStatusArgs());
}

static void svnclient_status_handler(void *baton, const char *path, svn_wc_status2_t *status)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnStatusArgs^ args = dynamic_cast<SvnStatusArgs^>(client->CurrentArgs); // C#: _currentArgs as SvnCommitArgs
	if(args)
	{
		args->OnStatus(gcnew SvnStatusEventArgs(SvnBase::Utf8_PtrToString(path), status));
	}
}

bool SvnClient::Status(SvnPathTarget^ path, EventHandler<SvnStatusEventArgs^>^ statusHandler, SvnStatusArgs^ args)
{
	if(!path)
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(_pool);
	_currentArgs = args;
	if(statusHandler)
		args->Status += statusHandler;
	try
	{
		svn_revnum_t version = 0;

		svn_opt_revision_t pegRev = path->Revision->ToSvnRevision();

		svn_error_t* err = svn_client_status2(&version,
			pool.AllocString(path->TargetName), 
			&pegRev, 
			svnclient_status_handler,
			(void*)_clientBatton->Handle,
			!args->NotRecursive,
			args->GetAll,
			args->Update,
			args->NoIgnore,
			args->IgnoreExternals,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(err);
	}
	finally
	{
		_currentArgs = nullptr;

		if(statusHandler)
			args->Status -= statusHandler;
	}
}

void SvnClient::GetStatus(SvnPathTarget^ path, [Out] IList<SvnStatusEventArgs^>^% statuses)
{
	throw gcnew NotImplementedException();
}

void SvnClient::GetStatus(SvnPathTarget^ path, [Out] SvnStatusEventArgs^% status)
{
	throw gcnew NotImplementedException();
}

bool SvnClient::GetStatus(SvnPathTarget^ path, SvnStatusArgs^ args, [Out] IList<SvnStatusEventArgs^>^% statuses)
{
	throw gcnew NotImplementedException();
}