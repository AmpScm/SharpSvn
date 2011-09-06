#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteDeletedRevisionArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;
using namespace System::Collections::Generic;

bool SvnRemoteSession::GetDeletedRevision(String^ relPath, SvnRevision^ revision, [Out] __int64% revno)
{
	if (!relPath)
		throw gcnew ArgumentNullException("relPath");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");

	return GetDeletedRevision(relPath, revision, gcnew SvnRemoteDeletedRevisionArgs(), revno);
}

bool SvnRemoteSession::GetDeletedRevision(String^ relPath, SvnRevision^ revision, SvnRemoteDeletedRevisionArgs^ args, [Out] __int64% revno)
{
	if (!relPath)
		throw gcnew ArgumentNullException("relPath");
	else if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	Ensure();
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	__int64 startrev, endrev;

	revno = SVN_INVALID_REVNUM;

	if (!InternalResolveRevision(revision, args, startrev)
		|| !InternalResolveRevision(args->EndRevision, args, endrev))
		return false;

	const char *pcPath = pool.AllocRelpath(relPath);
	svn_revnum_t deleted_rev;

	SVN_HANDLE(svn_ra_get_deleted_rev(_session, pcPath,
									  (svn_revnum_t)startrev,
									  (svn_revnum_t)endrev,
									  &deleted_rev,
									  pool.Handle));

	revno = deleted_rev;

	return true;
}
