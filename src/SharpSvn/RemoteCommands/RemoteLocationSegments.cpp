#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteLocationSegmentsArgs.h"


using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;
using namespace System::Collections::Generic;

bool SvnRemoteSession::LocationSegments(String^ relPath, EventHandler<SvnRemoteLocationSegmentEventArgs^>^ segmentHandler)
{
    if (!segmentHandler)
        throw gcnew ArgumentNullException("segmentHandler");

    return LocationSegments(relPath, gcnew SvnRemoteLocationSegmentsArgs(), segmentHandler);
}

static svn_error_t* svnsession_segment_receiver(svn_location_segment_t *segment, void *baton, apr_pool_t *pool)
{
    SvnRemoteSession^ session = AprBaton<SvnRemoteSession^>::Get((IntPtr)baton);

    AprPool thePool(pool, false);

    SvnRemoteLocationSegmentsArgs^ args = dynamic_cast<SvnRemoteLocationSegmentsArgs^>(session->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs
    if (!args)
        return nullptr;

    SvnRemoteLocationSegmentEventArgs^ e = gcnew SvnRemoteLocationSegmentEventArgs(segment, args->_reposUri, args->_nOffset);
    try
    {
        args->OnLocationSegment(e);

        if (e->Cancel)
            return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "LocationSegment receiver canceled operation");
        else
            return nullptr;
    }
    catch(Exception^ ex)
    {
        return SvnException::CreateExceptionSvnError("LocationSegment receiver", ex);
    }
    finally
    {
        e->Detach(false);
    }
}


bool SvnRemoteSession::LocationSegments(String^ relPath, SvnRemoteLocationSegmentsArgs^ args, EventHandler<SvnRemoteLocationSegmentEventArgs^>^ segmentHandler)
{
    Uri^ uri;
    if (!args)
        throw gcnew ArgumentNullException("args");

    if (String::IsNullOrEmpty(relPath))
        relPath = "";
    else if (!Uri::TryCreate(relPath, UriKind::Relative, uri))
        throw gcnew ArgumentException("Not a valid relative path", "relPath");

    Ensure();
    AprPool pool(%_pool);
    ArgsStore store(this, args, %pool);

    if (segmentHandler)
        args->LocationSegment += segmentHandler;
    try
    {
        const char *repos_root;
        const char *session_root;

        SVN_HANDLE(svn_ra_get_session_url(_session, &session_root, pool.Handle));
        SVN_HANDLE(svn_ra_get_repos_root2(_session, &repos_root, pool.Handle));

        args->_nOffset = strlen(session_root) - strlen(repos_root);

        if (!_reposRoot)
            _reposRoot = Utf8_PtrToUri(repos_root, SvnNodeKind::Directory);

        args->_reposUri = _reposRoot;

        SVN_HANDLE(svn_ra_get_location_segments(_session,
                                                pool.AllocRelpath(relPath),
                                                (svn_revnum_t)args->OperationalRevision,
                                                (svn_revnum_t)args->StartRevision,
                                                (svn_revnum_t)args->EndRevision,
                                                svnsession_segment_receiver,
                                                (void*)_clientBaton->Handle,
                                                pool.Handle));

        return true;
    }
    finally
    {
        if (segmentHandler)
            args->LocationSegment -= segmentHandler;

        args->_nOffset = 0;
        args->_reposUri = nullptr;
    }
}
