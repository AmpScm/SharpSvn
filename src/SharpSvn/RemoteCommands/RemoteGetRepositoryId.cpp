#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteCommonArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace System::Collections::Generic;

bool SvnRemoteSession::GetRepositoryId([Out] Guid% uuid)
{
	return GetRepositoryId(gcnew SvnRemoteCommonArgs(), uuid);
}

bool SvnRemoteSession::GetRepositoryId(SvnRemoteCommonArgs^ args, [Out] Guid% uuid)
{
	if (!args)
		throw gcnew ArgumentNullException("args");

	Ensure();
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	const char *id;
	uuid = Guid::Empty;

	SVN_HANDLE(svn_ra_get_uuid2(_session, &id, pool.Handle));

	if (id && *id)
		uuid = Guid(Utf8_PtrToString(id));

	return true;
}