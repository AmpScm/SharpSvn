#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteCommonArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;
using namespace System::Collections::Generic;

bool SvnRemoteSession::GetNodeKind(String^ relPath, [Out] SvnNodeKind% result)
{
	return GetNodeKind(relPath, gcnew SvnRemoteCommonArgs(), result);
}

bool SvnRemoteSession::GetNodeKind(String^ relPath, SvnRemoteCommonArgs^ args, [Out] SvnNodeKind% result)
{
	Uri^ uri;
    if (!args)
        throw gcnew ArgumentNullException("args");

    if (String::IsNullOrEmpty(relPath))
        relPath = "";
    else if (!Uri::TryCreate(relPath, UriKind::Relative, uri))
        throw gcnew ArgumentException("Not a valid relative path", "relPath");

	Ensure();
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	svn_node_kind_t kind;
	result = SvnNodeKind::Unknown;

	SVN_HANDLE(svn_ra_check_path(_session, pool.AllocCanonical(relPath), -1, &kind, pool.Handle));

	result = (SvnNodeKind)kind;

	return true;
}