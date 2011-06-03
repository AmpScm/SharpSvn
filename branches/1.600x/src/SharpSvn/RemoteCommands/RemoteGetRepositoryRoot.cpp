#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteCommonArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;
using namespace System::Collections::Generic;

bool SvnRemoteSession::GetRepositoryRoot([Out] Uri^% uri)
{
	return GetRepositoryRoot(gcnew SvnRemoteCommonArgs(), uri);
}

bool SvnRemoteSession::GetRepositoryRoot(SvnRemoteCommonArgs^ args, [Out] Uri^% uri)
{
	if (!args)
		throw gcnew ArgumentNullException("args");

	Ensure();
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	const char *url;
	uri = nullptr;

	SVN_HANDLE(svn_ra_get_repos_root2(_session, &url, pool.Handle));

	uri = Utf8_PtrToUri(url, SvnNodeKind::Directory);

	return true;
}