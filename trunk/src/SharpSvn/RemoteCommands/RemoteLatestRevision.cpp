#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteCommonArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;
using namespace System::Collections::Generic;

bool SvnRemoteSession::GetLatestRevision([Out] __int64% revision)
{
	return GetLatestRevision(gcnew SvnRemoteCommonArgs(), revision);
}

bool SvnRemoteSession::GetLatestRevision(SvnRemoteCommonArgs^ args, [Out] __int64% revision)
{
	return InternalGetLatestRevision(args, revision);
}

bool SvnRemoteSession::InternalGetLatestRevision(SvnRemoteSessionArgs^ args, [Out] __int64% revision)
{
	if (!args)
		throw gcnew ArgumentNullException("args");

	Ensure();
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	svn_revnum_t rev = -1;
	try
	{
		return args->HandleResult(this, svn_ra_get_latest_revnum(_session, &rev, pool.Handle));
	}
	finally
	{
		revision = rev;
	}
}