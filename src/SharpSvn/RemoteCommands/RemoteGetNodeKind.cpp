#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteNodeKindArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;
using namespace System::Collections::Generic;

bool SvnRemoteSession::GetNodeKind(String^ relPath, [Out] SvnNodeKind% result)
{
    if (!relPath)
        throw gcnew ArgumentNullException("relPath");

	return GetNodeKind(relPath, gcnew SvnRemoteNodeKindArgs(), result);
}

bool SvnRemoteSession::GetNodeKind(String^ relPath, SvnRemoteNodeKindArgs^ args, [Out] SvnNodeKind% result)
{
	if (!relPath)
        throw gcnew ArgumentNullException("relPath");
    else if (!args)
        throw gcnew ArgumentNullException("args");

	Ensure();
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	svn_node_kind_t kind;
	result = SvnNodeKind::Unknown;

	SVN_HANDLE(svn_ra_check_path(_session, pool.AllocRelpath(relPath), (svn_revnum_t)args->Revision, &kind, pool.Handle));

	result = (SvnNodeKind)kind;

	return true;
}