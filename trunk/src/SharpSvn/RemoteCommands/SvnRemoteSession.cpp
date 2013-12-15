#include "stdafx.h"

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


svn_error_t * SvnRemoteSession::Init()
{
    return nullptr;
}

void SvnRemoteSession::Ensure()
{
    if (!_session)
        throw gcnew InvalidOperationException(SharpSvnStrings::RemoteSessionMustBeOpenToPerformCommand);
}

void SvnRemoteSession::HandleClientCancel(SvnCancelEventArgs^ e)
{
    __super::HandleClientCancel(e);

    if (! e->Cancel)
        OnCancel(e);
}

void SvnRemoteSession::OnCancel(SvnCancelEventArgs^ e)
{
    Cancel(this, e);
}

void SvnRemoteSession::HandleClientProgress(SvnProgressEventArgs^ e)
{
    __super::HandleClientProgress(e);

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

    AprPool pool(%_pool);
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

    AprPool pool(%_pool);
    const char *result;

    SVN_THROW(svn_ra_get_path_relative_to_root(_session, &result, pool.AllocUri(uri), pool.Handle));

    return Utf8_PtrToString(result);
}
