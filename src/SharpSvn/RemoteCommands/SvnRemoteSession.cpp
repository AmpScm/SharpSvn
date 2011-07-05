#include "stdafx.h"

#include "SvnAll.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types


using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;

SvnRemoteSession::SvnRemoteSession()
	: _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
	_clientBaton = gcnew AprBaton<SvnRemoteSession^>(this);
	SVN_THROW(Init());
}

SvnRemoteSession::SvnRemoteSession(Uri^ sessionUri)
	: _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
	_clientBaton = gcnew AprBaton<SvnRemoteSession^>(this);
	SVN_THROW(Init());
	if (!Open(sessionUri))
	{
		// Should never happen, unless somebody suppresses the error in Open
		throw gcnew InvalidOperationException();
	}
}

SvnRemoteSession::~SvnRemoteSession()
{
	_session = nullptr;
	delete _clientBaton;
}

static svn_error_t *
svnremoteclient_cancel_func(void *cancel_baton)
{
	SvnRemoteSession^ client = AprBaton<SvnRemoteSession^>::Get((IntPtr)cancel_baton);

	SvnCancelEventArgs^ ea = gcnew SvnCancelEventArgs();
	try
	{
		client->HandleClientCancel(ea);

		if (ea->Cancel)
			return svn_error_create (SVN_ERR_CANCELLED, nullptr, "Operation canceled from OnCancel");

		return nullptr;
	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("Cancel function", e);
	}
	finally
	{
		ea->Detach(false);
	}
}

void
svnremoteclient_progress_notify_func(apr_off_t progress, apr_off_t total, void *baton, apr_pool_t *pool)
{
	UNUSED_ALWAYS(pool);
	SvnRemoteSession^ client = AprBaton<SvnRemoteSession^>::Get((IntPtr)baton);

	SvnProgressEventArgs^ ea = gcnew SvnProgressEventArgs(progress, total);

	try
	{
		client->HandleClientProgress(ea);
	}
	finally
	{
		ea->Detach(false);
	}
}

svn_error_t * SvnRemoteSession::Init()
{
	CtxHandle->cancel_func = svnremoteclient_cancel_func;
	CtxHandle->cancel_baton = (void*)_clientBaton->Handle;
	CtxHandle->progress_func = svnremoteclient_progress_notify_func;
	CtxHandle->progress_baton = (void*)_clientBaton->Handle;

	return nullptr;
}

void SvnRemoteSession::Ensure()
{
	if (!_session)
		throw gcnew InvalidOperationException(SharpSvnStrings::RemoteSessionMustBeOpenToPerformCommand);
}

void SvnRemoteSession::HandleClientCancel(SvnCancelEventArgs^ e)
{
	if (CurrentCommandArgs)
		CurrentCommandArgs->RaiseOnCancel(e);

	if (e->Cancel)
		return;

	OnCancel(e);
}

void SvnRemoteSession::OnCancel(SvnCancelEventArgs^ e)
{
	Cancel(this, e);
}

void SvnRemoteSession::HandleClientProgress(SvnProgressEventArgs^ e)
{
	if (CurrentCommandArgs)
		CurrentCommandArgs->RaiseOnProgress(e);

	OnProgress(e);
}

void SvnRemoteSession::OnProgress(SvnProgressEventArgs^ e)
{
	Progress(this, e);
}

void SvnRemoteSession::HandleClientError(SvnErrorEventArgs^ e)
{
	if (e->Cancel)
		return;

	if (CurrentCommandArgs)
	{
		CurrentCommandArgs->RaiseOnSvnError(e);

		if (e->Cancel)
			return;
	}

	OnSvnError(e);
}

void SvnRemoteSession::OnSvnError(SvnErrorEventArgs^ e)
{
	SvnError(this, e);
}

void SvnRemoteSession::HandleProcessing(SvnProcessingEventArgs^ e)
{
	OnProcessing(e);
}

void SvnRemoteSession::OnProcessing(SvnProcessingEventArgs^ e)
{
	Processing(this, e);
}

bool SvnRemoteSession::IsConnectionlessRepository(Uri^ uri)
{
	if (!uri)
		throw gcnew ArgumentNullException("uri");
	else if (!uri->IsAbsoluteUri)
		throw gcnew ArgumentException(SharpSvnStrings::UriIsNotAbsolute, "uri");

	String^ scheme = uri->Scheme->ToUpperInvariant();

	if (scheme == "FILE" || scheme == "HTTP" || scheme == "HTTPS")
		return true;

	return false;
}

bool SvnRemoteSession::RequiresExternalAuthorization(Uri^ uri)
{
	if (!uri)
		throw gcnew ArgumentNullException("uri");
	else if (!uri->IsAbsoluteUri)
		throw gcnew ArgumentException(SharpSvnStrings::UriIsNotAbsolute, "uri");

	String^ scheme = uri->Scheme->ToUpperInvariant();

	if (scheme == "FILE" || scheme == "HTTP" || scheme == "HTTPS" || scheme == "SVN")
		return false;

	return true;
}

String^ SvnRemoteSession::MakeRelativePath(Uri^ uri)
{
	if (!uri)
		throw gcnew ArgumentNullException("uri");
	else if (!uri->IsAbsoluteUri)
		throw gcnew ArgumentException(SharpSvnStrings::UriIsNotAbsolute, "uri");

	AprPool pool(SmallThreadPool);
	const char *result;

	SVN_THROW(svn_ra_get_path_relative_to_session(_session, &result, pool.AllocUri(uri), pool.Handle));

	return Utf8_PtrToString(result);
}

String^ SvnRemoteSession::MakeRepositoryRootRelativePath(Uri^ uri)
{
	if (!uri)
		throw gcnew ArgumentNullException("uri");
	else if (!uri->IsAbsoluteUri)
		throw gcnew ArgumentException(SharpSvnStrings::UriIsNotAbsolute, "uri");

	AprPool pool(SmallThreadPool);
	const char *result;

	SVN_THROW(svn_ra_get_path_relative_to_root(_session, &result, pool.AllocUri(uri), pool.Handle));

	return Utf8_PtrToString(result);
}
