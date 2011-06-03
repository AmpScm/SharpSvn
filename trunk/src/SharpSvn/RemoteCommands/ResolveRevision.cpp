#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteCommonArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;
using namespace System::Collections::Generic;


bool SvnRemoteSession::ResolveRevision(SvnRevision^ revision, [Out] __int64% revno)
{
	if (!revision)
		throw gcnew ArgumentNullException("revision");

	return ResolveRevision(revision, gcnew SvnRemoteCommonArgs(), revno);
}

bool SvnRemoteSession::ResolveRevision(SvnRevision^ revision, SvnRemoteCommonArgs^ args, [Out] __int64% revno)
{
	if (!revision)
		throw gcnew ArgumentNullException("revision");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	switch (revision->RevisionType)
	{
	case SvnRevisionType::Head:
		return GetLatestRevision(args, revno);

	case SvnRevisionType::Number:
		revno = revision->Revision;
		return true;

	case SvnRevisionType::Time:
		break;

	default:
		throw gcnew ArgumentException(SharpSvnStrings::CanOnlyResolveHeadDateandExplicitRevisions, "revision");
	}

	Ensure();
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);
	revno = -1L;
	svn_revnum_t rev;
	svn_opt_revision_t opt_rev = revision->ToSvnRevision();

	if (args->HandleResult(this, svn_ra_get_dated_revision(_session, &rev, opt_rev.value.date, pool.Handle)))
	{
		revno = rev;
		return true;
	}

	return false;
}