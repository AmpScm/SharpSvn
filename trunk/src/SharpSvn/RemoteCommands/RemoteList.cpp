#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteListArgs.h"


using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;
using namespace System::Collections::Generic;

bool SvnRemoteSession::List(String^ relPath, EventHandler<SvnRemoteListEventArgs^>^ listHandler)
{
	if (!listHandler)
		throw gcnew ArgumentNullException("listHandler");

	return List(relPath, gcnew SvnRemoteListArgs(), listHandler);
}

bool SvnRemoteSession::List(String^ relPath, SvnRemoteListArgs^ args, EventHandler<SvnRemoteListEventArgs^>^ listHandler)
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

	if (listHandler)
		args->List += listHandler;
	try
	{
		apr_hash_t *dirents;
		svn_revnum_t gotRev;

		SVN_HANDLE(svn_ra_get_dir2(_session, &dirents, &gotRev, NULL, pool.AllocRelpath(relPath),
								   (svn_revnum_t)args->Revision, (apr_uint32_t)args->RetrieveEntries, pool.Handle));

		if (!String::IsNullOrEmpty(relPath) && !relPath->EndsWith("/"))
			relPath += "/";

		for (apr_hash_index_t *hi = apr_hash_first(pool.Handle, dirents); hi; hi = apr_hash_next(hi))
		{
			const char* pKey;
			apr_ssize_t keyLen;
			const svn_dirent_t *dirent;

			apr_hash_this(hi, (const void**)&pKey, &keyLen, (void**)&dirent);
			SvnRemoteListEventArgs^ ea = gcnew SvnRemoteListEventArgs(Utf8_PtrToString(pKey, keyLen), dirent, gotRev, SessionUri, relPath);
			try
			{
				args->OnList(ea);
			}
			finally
			{
				ea->Detach(false);
			}
		}

		return true;
	}
	finally
	{
		if (listHandler)
			args->List -= listHandler;
	}
}
