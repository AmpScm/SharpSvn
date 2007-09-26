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

	Status(path, gcnew SvnStatusArgs(), statusHandler);
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

bool SvnClient::Status(String^ path, SvnStatusArgs^ args, EventHandler<SvnStatusEventArgs^>^ statusHandler)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	// We allow a null statusHandler; the args object might just handle it itself

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if(statusHandler)
		args->Status += statusHandler;
	try
	{
		svn_revnum_t version = 0;

		svn_opt_revision_t pegRev = args->Revision->ToSvnRevision();

		svn_error_t* r = svn_client_status3(&version,
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

		return args->HandleResult(this, r);
	}
	finally
	{
		if(statusHandler)
			args->Status -= statusHandler;
	}
}

void SvnClient::GetStatus(String^ path, [Out] IList<SvnStatusEventArgs^>^% statuses)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	InfoItemCollection<SvnStatusEventArgs^>^ results = gcnew InfoItemCollection<SvnStatusEventArgs^>();

	try
	{
		Status(path, gcnew SvnStatusArgs(), results->Handler);
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

	InfoItemCollection<SvnStatusEventArgs^>^ results = gcnew InfoItemCollection<SvnStatusEventArgs^>();

	SvnStatusArgs^ args = gcnew SvnStatusArgs();
	args->Depth = SvnDepth::Empty;
	args->GetAll = true;
	args->NoIgnore = true;

	try
	{
		Status(path, args, results->Handler);
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

	InfoItemCollection<SvnStatusEventArgs^>^ results = gcnew InfoItemCollection<SvnStatusEventArgs^>();

	try
	{
		return Status(path, args, results->Handler);
	}
	finally
	{
		statuses = safe_cast<IList<SvnStatusEventArgs^>^>(results);
	}
}