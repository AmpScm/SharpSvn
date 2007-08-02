#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::Status(String^ path, EventHandler<SvnStatusEventArgs^>^ statusHandler)
{
	if(String::IsNullOrEmpty(path))
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
		SvnStatusEventArgs^ e = gcnew SvnStatusEventArgs(SvnBase::Utf8_PtrToString(path), status);
		try
		{
			args->OnStatus(e);
		}
		finally
		{
			e->Detach(false);
		}
	}
}

bool SvnClient::Status(String^ path, EventHandler<SvnStatusEventArgs^>^ statusHandler, SvnStatusArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!_pool)
		throw gcnew ObjectDisposedException("SvnClient");
	else if(!IsNotUri(path))
		throw gcnew ArgumentException("Path must be a local path", "path");

	// We allow a null statusHandler; the args object might just handle it itself

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

		svn_opt_revision_t pegRev = args->Revision->ToSvnRevision();

		svn_error_t* err = svn_client_status3(&version,
			pool.AllocPath(path), 
			&pegRev, 
			svnclient_status_handler,
			(void*)_clientBatton->Handle,
			(svn_depth_t)args->Depth,
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

void SvnClient::GetStatus(String^ path, [Out] IList<SvnStatusEventArgs^>^% statuses)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	InfoItemList<SvnStatusEventArgs^>^ results = gcnew InfoItemList<SvnStatusEventArgs^>();

	try
	{
		Status(path, results->Handler, gcnew SvnStatusArgs());	
	}
	finally
	{
		statuses = safe_cast<IList<SvnStatusEventArgs^>^>(results);
	}
}

void SvnClient::GetStatus(String^ path, [Out] SvnStatusEventArgs^% status)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	InfoItemList<SvnStatusEventArgs^>^ results = gcnew InfoItemList<SvnStatusEventArgs^>();

	SvnStatusArgs^ args = gcnew SvnStatusArgs();
	args->Depth = SvnDepth::Empty;
	args->GetAll = true;
	args->NoIgnore = true;

	try
	{
		Status(path, results->Handler, args);
	}
	finally
	{
		if(results->Count > 0)
			status = results[0];
		else
			status = nullptr;
	}
}

bool SvnClient::GetStatus(String^ path, SvnStatusArgs^ args, [Out] IList<SvnStatusEventArgs^>^% statuses)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	InfoItemList<SvnStatusEventArgs^>^ results = gcnew InfoItemList<SvnStatusEventArgs^>();

	try
	{
		return Status(path, results->Handler, args);	
	}
	finally
	{
		statuses = safe_cast<IList<SvnStatusEventArgs^>^>(results);
	}
}