#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemotePropertiesArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;
using namespace System::Collections::Generic;

bool SvnRemoteSession::GetProperties(String^ relpath, [Out] SvnPropertyCollection^% properties)
{
    if (!relpath)
        throw gcnew ArgumentNullException("relpath");

	return GetProperties(relpath, gcnew SvnRemotePropertiesArgs(), properties);
}

bool SvnRemoteSession::GetProperties(String^ relpath, SvnRemotePropertiesArgs^ args, [Out] SvnPropertyCollection^% properties)
{
    if (!relpath)
        throw gcnew ArgumentNullException("relpath");
    else if (!args)
        throw gcnew ArgumentNullException("args");

	Ensure();
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	svn_node_kind_t kind;
    const char *pRelPath = pool.AllocRelpath(relpath);

    svn_revnum_t rev;
    apr_hash_t *props = NULL;

    properties = nullptr;

    if (args->Revision > 0)
        rev = (svn_revnum_t)args->Revision;
    else
        SVN_HANDLE(svn_ra_get_latest_revnum(_session, &rev, pool.Handle));

    if (args->NodeKind == SvnNodeKind::File || args->NodeKind == SvnNodeKind::Directory)
        kind = (svn_node_kind_t)args->NodeKind;
    else
        SVN_HANDLE(svn_ra_check_path(_session, pRelPath, rev, &kind, pool.Handle));

    if (kind == svn_node_file)
        SVN_HANDLE(svn_ra_get_file(_session, pRelPath, rev, nullptr, nullptr, &props, pool.Handle));
    else if (kind == svn_node_dir)
        SVN_HANDLE(svn_ra_get_dir2(_session, nullptr, nullptr, &props, pRelPath, rev, 0, pool.Handle));
    else
        return args->HandleResult(this, svn_error_create(SVN_ERR_FS_NOT_FOUND, NULL, NULL), relpath);

    if (!props)
        return true;

    /* Remove DAV and entry properties from hash */
    apr_array_header_t *all_props = svn_prop_hash_to_array(props, pool.Handle);
    apr_array_header_t *regular_props;
    SVN_HANDLE(svn_categorize_props(all_props, nullptr, nullptr, &regular_props, pool.Handle));

    properties = CreatePropertyDictionary(svn_prop_array_to_hash(regular_props, pool.Handle), %pool);

	return true;
}