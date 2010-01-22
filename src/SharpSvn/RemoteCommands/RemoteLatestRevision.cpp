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
	if (!args)
		throw gcnew ArgumentNullException("args");

	Ensure();
	ArgsStore store(this, args);
	AprPool pool(%_pool);

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