#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::Log(SvnTarget^ target, EventHandler<SvnLogEventArgs^>^ logHandler)
{

}

bool SvnClient::Log(SvnTarget^ target, EventHandler<SvnLogEventArgs^>^ logHandler, SvnLogArgs^ args)
{
	return false;
}

void SvnClient::Log(ICollection<Uri^>^ targets, SvnRevision^ pegRevision, EventHandler<SvnLogEventArgs^>^ logHandler)
{

}

void SvnClient::Log(ICollection<Uri^>^ targets, SvnRevision^ pegRevision, EventHandler<SvnLogEventArgs^>^ logHandler, SvnLogArgs^ args)
{

}

void SvnClient::Log(ICollection<String^>^ targets, SvnRevision^ pegRevision, EventHandler<SvnLogEventArgs^>^ logHandler)
{

}

void SvnClient::Log(ICollection<String^>^ targets, SvnRevision^ pegRevision, EventHandler<SvnLogEventArgs^>^ logHandler, SvnLogArgs^ args)
{

}


void SvnClient::GetLog(SvnTarget^ target, [Out] IList<SvnLogEventArgs^>^% logHandler)
{

}

bool SvnClient::GetLog(SvnTarget^ target, SvnLogArgs^ args, [Out] IList<SvnLogEventArgs^>^% logHandler)
{
	return false;
}

void SvnClient::GetLog(ICollection<Uri^>^ targets, SvnRevision^ pegRevision, [Out] IList<SvnLogEventArgs^>^% logHandler)
{

}

bool SvnClient::GetLog(ICollection<Uri^>^ targets, SvnRevision^ pegRevision, SvnLogArgs^ args, [Out] IList<SvnLogEventArgs^>^% logHandler)
{
	return false;
}

void SvnClient::GetLog(ICollection<String^>^ targets, SvnRevision^ pegRevision, [Out] IList<SvnLogEventArgs^>^% logHandler)
{

}

bool SvnClient::GetLog(ICollection<String^>^ targets, SvnRevision^ pegRevision, SvnLogArgs^ args, [Out] IList<SvnLogEventArgs^>^% logHandler)
{
	return false;

}


static svn_error_t *svnclient_log_handler(void *baton, apr_hash_t *changed_paths, svn_revnum_t revision, const char *author, const char *date, const char *message, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);

	SvnLogArgs^ args = dynamic_cast<SvnLogArgs^>(client->CurrentArgs); // C#: _currentArgs as SvnLogArgs
	if(args)
	{
		SvnLogEventArgs^ e = gcnew SvnLogEventArgs();

		/* date: use svn_time_from_cstring() if need apr_time_t */
		try
		{
			args->OnLog(e);
		}
		finally
		{
			e->Detach(false);
		}
	}
	return NULL;
}

bool SvnClient::InternalLog(ICollection<String^>^ targetStrings, SvnRevision^ pegRevision, SvnLogArgs^ args, EventHandler<SvnLogEventArgs^>^ logHandler)
{
	if(!targetStrings)
		throw gcnew ArgumentNullException("targetStrings");
	else if(!pegRevision)
		throw gcnew ArgumentNullException("pegRevision");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!_pool)
		throw gcnew ObjectDisposedException("SvnClient");

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(_pool);
	_currentArgs = args;

	if(logHandler)
		args->Log += logHandler;
	try
	{
		AprArray<String^, AprCStrMarshaller^>^ targets = gcnew AprArray<String^, AprCStrMarshaller^>(targetStrings, %pool);

		svn_opt_revision_t pegRev = pegRevision->ToSvnRevision();
		svn_opt_revision_t start = args->Start->ToSvnRevision();
		svn_opt_revision_t end = args->End->ToSvnRevision();

		svn_error_t *r = svn_client_log3(
			targets->Handle,
			&pegRev,
			&start,
			&end,
			args->Limit,
			args->LogChangedPaths,
			args->StrictHistory,
			svnclient_log_handler,
			(void*)_clientBatton->Handle,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;

		if(logHandler)
			args->Log -= logHandler;
	}
}