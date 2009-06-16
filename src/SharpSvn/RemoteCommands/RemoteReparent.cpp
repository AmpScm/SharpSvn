#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteSessionReparentArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace System::Collections::Generic;

bool SvnRemoteSession::Reparent(Uri^ sessionUri)
{
	if (!sessionUri)
		throw gcnew ArgumentNullException("sessionUri");

	return Reparent(sessionUri, gcnew SvnRemoteSessionReparentArgs());
}

bool SvnRemoteSession::Reparent(Uri^ sessionUri, SvnRemoteSessionReparentArgs^ args)
{
	if (!sessionUri)
		throw gcnew ArgumentNullException("sessionUri");

	Ensure();
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	return args->HandleResult(this, svn_ra_reparent(_session, pool.AllocCanonical(sessionUri), pool.Handle));
}