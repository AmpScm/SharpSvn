#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteListArgs.h"


using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace System::Collections::Generic;

bool SvnRemoteSession::List(String^ relPath,  __int64 revision, EventHandler<SvnRemoteListEventArgs^>^ listHandler)
{
	if (!listHandler)
		throw gcnew ArgumentNullException("listHandler");

	return List(relPath, revision, gcnew SvnRemoteListArgs(), listHandler);
}

bool SvnRemoteSession::List(String^ relPath,  __int64 revision, SvnRemoteListArgs^ args, EventHandler<SvnRemoteListEventArgs^>^ listHandler)
{
    Uri^ uri;
    if (!args)
        throw gcnew ArgumentNullException("args");

    if (String::IsNullOrEmpty(relPath))
        relPath = "";
    else if (!Uri::TryCreate(relPath, UriKind::Relative, uri))
        throw gcnew ArgumentException("Not a valid relative path", "relPath");

    Ensure();
    ArgsStore store(this, args);
    AprPool pool(%_pool);

	if (listHandler)
		args->List += listHandler;
	try
	{
		apr_hash_t *dirents;

		SVN_HANDLE(svn_ra_get_dir2(_session, &dirents, NULL, NULL, pool.AllocCanonical(relPath), (svn_revnum_t)revision, (apr_uint32_t)args->RetrieveEntries, pool.Handle));

		for (apr_hash_index_t *hi = apr_hash_first(pool.Handle, dirents); hi; hi = apr_hash_next(hi))
		{
			const char* pKey;
			apr_ssize_t keyLen;
			const svn_dirent_t *dirent;

			apr_hash_this(hi, (const void**)&pKey, &keyLen, (void**)&dirent);
			SvnRemoteListEventArgs^ ea = gcnew SvnRemoteListEventArgs(Utf8_PtrToString(pKey, keyLen), dirent);
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
