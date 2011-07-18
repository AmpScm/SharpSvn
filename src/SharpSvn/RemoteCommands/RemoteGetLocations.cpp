#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteLocationArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;
using namespace System::Collections::Generic;

bool SvnRemoteSession::GetLocations(String^ relpath, ICollection<__int64>^ resolveRevisions, [Out] SvnRevisionLocationMap^% locations)
{
	if (!relpath)
		throw gcnew ArgumentNullException("relpath");
	else if (!resolveRevisions)
		throw gcnew ArgumentNullException("resolveRevisions");
	else if (resolveRevisions->Count == 0)
		throw gcnew ArgumentException(SharpSvnStrings::CollectionMustContainAtLeastOneItem, "resolveRevisions");

	return GetLocations(relpath, resolveRevisions, gcnew SvnRemoteLocationArgs(), locations);
}

bool SvnRemoteSession::GetLocations(String^ relpath, ICollection<__int64>^ resolveRevisions, SvnRemoteLocationArgs^ args, [Out] SvnRevisionLocationMap^% locations)
{
	if (!relpath)
		throw gcnew ArgumentNullException("relpath");
	else if (!resolveRevisions)
		throw gcnew ArgumentNullException("resolveRevisions");
	else if (resolveRevisions->Count == 0)
		throw gcnew ArgumentException(SharpSvnStrings::CollectionMustContainAtLeastOneItem, "resolveRevisions");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	Ensure();
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	bool have_kind = false;
	svn_node_kind_t kind;
	apr_hash_t *locs;
	apr_array_header_t *revs = apr_array_make(pool.Handle, resolveRevisions->Count, sizeof(svn_revnum_t));
	const char *repos_root;
	const char *pcPath = pool.AllocRelpath(relpath);
    svn_revnum_t rev;

    if (args->Revision >= 0)
        rev = (svn_revnum_t)args->Revision;
    else
        SVN_HANDLE(svn_ra_get_latest_revnum(_session, &rev, pool.Handle));

	for each (__int64 rr in resolveRevisions)
	{
		APR_ARRAY_PUSH(revs, svn_revnum_t) = (svn_revnum_t)rr;
	}

	SVN_HANDLE(svn_ra_get_repos_root2(_session, &repos_root, pool.Handle));

	SVN_HANDLE(svn_ra_get_locations(_session,
									&locs,
									pcPath,
									rev,
									revs,
									pool.Handle));

	locations = gcnew SvnRevisionLocationMap();

	for (int i = 0; i < revs->nelts; i++)
	{
		svn_revnum_t *revPtr = &APR_ARRAY_IDX(revs, i, svn_revnum_t);
		const char *url = (const char *)apr_hash_get(locs, revPtr, sizeof(svn_revnum_t));

		if (url)
		{
			if (!have_kind)
			{
				have_kind = true;
				SVN_HANDLE(svn_ra_check_path(_session, pcPath, rev, &kind, pool.Handle));
			}

			const char *pUri = svn_path_url_add_component2(repos_root, url+1, pool.Handle);

			locations->Add(gcnew SvnUriTarget(Utf8_PtrToUri(pUri, (SvnNodeKind)kind), *revPtr));
		}
	}

	return true;
}
