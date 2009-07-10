#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteSessionLatestRevisionArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace System::Collections::Generic;

bool SvnRemoteSession::GetLatestRevision([Out] __int64% revision)
{
	return GetLatestRevision(gcnew SvnRemoteSessionLatestRevisionArgs(), revision);
}

bool SvnRemoteSession::GetLatestRevision(SvnRemoteSessionLatestRevisionArgs^ args, [Out] __int64% revision)
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