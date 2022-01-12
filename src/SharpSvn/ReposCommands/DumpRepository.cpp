// Copyright 2008-2009 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "stdafx.h"

#include "SvnStreamWrapper.h"
#include "ReposArgs/DumpRepository.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace System::IO;
using namespace SharpSvn;
using namespace SharpSvn::Implementation;

bool SvnRepositoryClient::DumpRepository(String^ repositoryPath, Stream^ to)
{
    if (String::IsNullOrEmpty(repositoryPath))
        throw gcnew ArgumentNullException("repositoryPath");
    else if (!to)
        throw gcnew ArgumentNullException("to");

    return DumpRepository(repositoryPath, to, gcnew SvnDumpRepositoryArgs());
}

static void
warning_func(void *baton, svn_error_t *err)
{
    /* NOOP: Default handler aborts */
    UNUSED_ALWAYS(baton);
    UNUSED_ALWAYS(err);
}

/* Set *REVNUM to the revision specified by REVISION (or to
SVN_INVALID_REVNUM if that has the type 'unspecified'),
possibly making use of the YOUNGEST revision number in REPOS. */
static svn_error_t *
get_revnum(svn_revnum_t *revnum, const svn_opt_revision_t *revision,
                   svn_revnum_t youngest, svn_repos_t *repos, apr_pool_t *pool)
{
    switch(revision->kind)
    {
    case svn_opt_revision_number:
        *revnum = revision->value.number;
        break;
    case svn_opt_revision_head:
        *revnum = youngest;
        break;
    case svn_opt_revision_date:
        SVN_ERR(svn_repos_dated_revision(revnum, repos, revision->value.date, pool));
        break;
    default:
        return svn_error_create(SVN_ERR_CL_ARG_PARSING_ERROR, NULL, "Invalid revision specifier");
    }

    if (*revnum > youngest)
        return svn_error_createf
        (SVN_ERR_CL_ARG_PARSING_ERROR, NULL, "Revisions must not be greater than the youngest revision (%ld)",
        youngest);

    return SVN_NO_ERROR;
}

bool SvnRepositoryClient::DumpRepository(String^ repositoryPath, Stream^ to, SvnDumpRepositoryArgs^ args)
{
    if (String::IsNullOrEmpty(repositoryPath))
        throw gcnew ArgumentNullException("repositoryPath");
    else if (!to)
        throw gcnew ArgumentNullException("to");
    else if (!args)
        throw gcnew ArgumentNullException("args");
    else if (!IsNotUri(repositoryPath))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");

    EnsureState(SvnContextState::ConfigLoaded);
    AprPool pool(%_pool);
    ArgsStore store(this, args, %pool);

    // based on subversion/svnadmin/main.c:

    svn_repos_t* repos = nullptr;
    svn_error_t* r;
    svn_revnum_t youngest;

    if (r = svn_repos_open3(&repos, pool.AllocDirent(repositoryPath), nullptr,
                            pool.Handle, pool.Handle))
        return args->HandleResult(this, r);

    svn_fs_t *fs = svn_repos_fs(repos); // Always ok

    // Set a simple warning handler (see subversion/svnadmin/main.c:open_repos), otherwise we might abort()
    svn_fs_set_warning_func(svn_repos_fs(repos), warning_func, nullptr);

    r = svn_fs_youngest_rev(&youngest, fs, pool.Handle);
    if (r)
        return args->HandleResult(this, r);

    svn_revnum_t start;
    svn_revnum_t end;

    r = get_revnum(&start, args->Start->Or(SvnRevision::Zero)->AllocSvnRevision(%pool), youngest, repos, pool.Handle);
    if (r)
        return args->HandleResult(this, r);
    r = get_revnum(&end, args->End->Or(SvnRevision::Head)->AllocSvnRevision(%pool), youngest, repos, pool.Handle);
    if (r)
        return args->HandleResult(this, r);

    if (start > end)
        return args->HandleResult(this, svn_error_create(SVN_ERR_CL_ARG_PARSING_ERROR, NULL, "First revision cannot be higher than second"));

    SvnStreamWrapper strmTo(to, false, true, %pool);

    r = svn_repos_dump_fs4(
        repos,
        strmTo.Handle,
        start,
        end,
        args->Incremental,
        args->Deltas,
        true /* include_revprops */,
        true /* include_changes */,
        repos_notify_func,
        _clientBaton->Handle,
        nullptr /* filter_func */,
        nullptr /* filter_baton */,
        CtxHandle->cancel_func,
        CtxHandle->cancel_baton,
        pool.Handle);

    return args->HandleResult(this, r);
}
