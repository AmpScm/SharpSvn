#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteCommonArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;
using namespace System::Collections::Generic;

bool SvnRemoteSession::Reparent(Uri^ sessionUri)
{
	if (!sessionUri)
		throw gcnew ArgumentNullException("sessionUri");

	return Reparent(sessionUri, gcnew SvnRemoteCommonArgs());
}

bool SvnRemoteSession::Reparent(Uri^ sessionUri, SvnRemoteCommonArgs^ args)
{
	if (!sessionUri)
		throw gcnew ArgumentNullException("sessionUri");

	Ensure();
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	if (args->HandleResult(this, svn_ra_reparent(_session, pool.AllocCanonical(sessionUri), pool.Handle)))
	{
		_root = nullptr;
		return true;
	}

	return false;
}