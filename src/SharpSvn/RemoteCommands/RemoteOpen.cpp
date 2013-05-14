#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteOpenArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;
using namespace System::Collections::Generic;

bool SvnRemoteSession::Open(Uri^ sessionUri)
{
	if (!sessionUri)
		throw gcnew ArgumentNullException("sessionUri");

	return Open(sessionUri, gcnew SvnRemoteOpenArgs());
}

bool SvnRemoteSession::Open(Uri^ sessionUri, SvnRemoteOpenArgs^ args)
{
	if (!sessionUri)
		throw gcnew ArgumentNullException("sessionUri");
	else if (!args)
		throw gcnew ArgumentNullException("args");
    else if (_session)
        throw gcnew InvalidOperationException(SharpSvnStrings::SessionAlreadyOpen);

	EnsureState(SvnContextState::AuthorizationInitialized);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);
	const char *psession_uri = pool.AllocUri(sessionUri);

	svn_ra_session_t *session;

	SVN_HANDLE(svn_client_open_ra_session2(&session,
										  psession_uri,
										  nullptr /* wri_abspath */,
										  CtxHandle,
										  _pool.Handle, // Use session pool!
										  pool.Handle));

	_session = session;
	_sessionRoot = Utf8_PtrToUri(psession_uri, SvnNodeKind::Directory);
	return true;
}
