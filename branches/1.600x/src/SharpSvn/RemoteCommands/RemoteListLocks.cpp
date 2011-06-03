#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteListLocksArgs.h"


using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;
using namespace System::Collections::Generic;

bool SvnRemoteSession::ListLocks(String^ relPath, EventHandler<SvnRemoteListLockEventArgs^>^ listHandler)
{
	if (!listHandler)
		throw gcnew ArgumentNullException("listHandler");

	return ListLocks(relPath, gcnew SvnRemoteListLocksArgs(), listHandler);
}

bool SvnRemoteSession::ListLocks(String^ relPath, SvnRemoteListLocksArgs^ args, EventHandler<SvnRemoteListLockEventArgs^>^ listHandler)
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
		apr_hash_t *locks;
        const char *repos_root;
        const char *session_root;
        int nOffset;

        SVN_HANDLE(svn_ra_get_session_url(_session, &session_root, pool.Handle));
        SVN_HANDLE(svn_ra_get_repos_root2(_session, &repos_root, pool.Handle));

        nOffset = strlen(session_root) - strlen(repos_root);

        SVN_HANDLE(svn_ra_get_locks(_session, &locks, pool.AllocCanonical(relPath), pool.Handle));

		for (apr_hash_index_t *hi = apr_hash_first(pool.Handle, locks); hi; hi = apr_hash_next(hi))
		{
			const char* pKey;
			apr_ssize_t keyLen;
			const svn_lock_t *lock;

			apr_hash_this(hi, (const void**)&pKey, &keyLen, (void**)&lock);
			SvnRemoteListLockEventArgs^ ea = gcnew SvnRemoteListLockEventArgs(Utf8_PtrToString(pKey+1, keyLen-1), lock, SessionUri, nOffset);
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
