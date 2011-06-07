#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteStatArgs.h"
#include "EventArgs/SvnRemoteStatEventArgs.h"


using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;
using namespace System::Collections::Generic;

bool SvnRemoteSession::GetStat(String^ relPath, [Out] SvnRemoteStatEventArgs^% result)
{
    return GetStat(relPath, gcnew SvnRemoteStatArgs(), result);
}

bool SvnRemoteSession::GetStat(String^ relPath, SvnRemoteStatArgs^ args, [Out] SvnRemoteStatEventArgs^% result)
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

    result = nullptr;

    const char *path = pool.AllocUri(relPath);
    svn_dirent_t *dirent = nullptr;
    svn_error_t *err = svn_ra_stat(_session,
                                   path,
                                   (svn_revnum_t)args->Revision,
                                   &dirent,
                                   pool.Handle);

    if (err && err->apr_err == SVN_ERR_RA_NOT_IMPLEMENTED)
    {
        // Handle SvnServe 1.0 and 1.1:
        Uri^ reposRoot;
        const char *repos_root;
        svn_revnum_t latest;

        svn_error_clear(err);
        err = nullptr;

        SVN_HANDLE(svn_ra_get_repos_root2(_session, &repos_root, pool.Handle));
        SVN_HANDLE(svn_ra_get_latest_revnum(_session, &latest, pool.Handle));

        reposRoot = Utf8_PtrToUri(repos_root, SvnNodeKind::Directory);

        if (String::IsNullOrEmpty(relPath) && reposRoot == this->SessionUri)
        {
            // We can't get the directory above the repository root,
            // but we can get the required details
            dirent = (svn_dirent_t*)pool.AllocCleared(sizeof(*dirent));

            dirent->created_rev = latest; // Per definition

            apr_hash_t *props;

            SVN_HANDLE(svn_ra_get_dir2(_session, nullptr, nullptr, &props, "", latest, 0, pool.Handle));

            dirent->has_props = (props != nullptr);

            SVN_HANDLE(svn_ra_rev_proplist(_session, latest, &props, pool.Handle));

            const svn_string_t *val;

            val = (const svn_string_t*)apr_hash_get(props, SVN_PROP_REVISION_DATE,
                                                    APR_HASH_KEY_STRING);
            if (val)
                SVN_HANDLE(svn_time_from_cstring(&dirent->time, val->data,
                                                 pool.Handle));
            else
                dirent->time = 0;

            val = (const svn_string_t*)apr_hash_get(props, SVN_PROP_REVISION_AUTHOR,
                                                    APR_HASH_KEY_STRING);
            dirent->last_author = val ? val->data : NULL;
        }
        else
        {
            const char *url;
            const char *basename;

            if (!String::IsNullOrEmpty(relPath))
                url = svn_path_url_add_component2(pool.AllocUri(SessionUri), pool.AllocRelpath(relPath), pool.Handle);
            else
                url = pool.AllocUri(SessionUri);

            svn_path_split(url, &url, &basename, pool.Handle); // 1.7: Use svn_uri_split()

            svn_ra_session_t *subsession;
            apr_hash_t *entries;

            SVN_HANDLE(svn_client_open_ra_session(&subsession, url, CtxHandle, pool.Handle));
            SVN_HANDLE(svn_ra_get_dir2(subsession, &entries, NULL, NULL, "", latest, SVN_DIRENT_ALL, pool.Handle));

            basename = svn_path_uri_decode(basename, pool.Handle);
            dirent = (svn_dirent_t*)apr_hash_get(entries, basename, APR_HASH_KEY_STRING);
        }
    }

    SVN_HANDLE(err);

    if (dirent)
    {
        result = gcnew SvnRemoteStatEventArgs(dirent);
        result->Detach();
        return true;
    }

    return false;
}
