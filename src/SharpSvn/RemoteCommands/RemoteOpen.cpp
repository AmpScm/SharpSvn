#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteOpenArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
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

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_ra_session_t *session;

	SVN_HANDLE(svn_client_open_ra_session(&session,
										  pool.AllocCanonical(sessionUri),
										  CtxHandle,
										  _pool.Handle // Use session pool!
										  ));

	_session = session;
	_root = nullptr;
	return true;
}

Uri^ SvnRemoteSession::SessionUri::get()
{
	if (_root)
		return _root;
	else if (!_session)
		return nullptr;

	AprPool pool(%_pool);
	const char *url;
	SVN_THROW(svn_ra_get_session_url(_session, &url, pool.Handle));

	_root = Utf8_PtrToUri(url, SvnNodeKind::Directory);

	return _root;
}
