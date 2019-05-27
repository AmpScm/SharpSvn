// Copyright 2007-2009 The SharpSvn Project
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

#include "Args/Youngest.h"

#include "private/svn_client_private.h"


using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::Youngest(String^ target, [Out] __int64 %revision)
{
    if (String::IsNullOrEmpty(target))
        throw gcnew ArgumentNullException("target");

    return Youngest(target, gcnew SvnYoungestArgs(), revision);
}

bool SvnClient::Youngest(String^ target, SvnYoungestArgs ^args, [Out] __int64 %revision)
{
    if (String::IsNullOrEmpty(target))
        throw gcnew ArgumentNullException("target");
    else if (!SvnBase::IsNotUri(target))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");

    revision = SVN_INVALID_REVNUM;

    EnsureState(SvnContextState::AuthorizationInitialized);
    AprPool pool(%_pool);
    ArgsStore store(this, args, %pool);

    svn_ra_session_t *session;
    svn_opt_revision_t rev;
    rev.kind = svn_opt_revision_unspecified;

    const char *path = pool.AllocAbsoluteDirent(target);

    SVN_HANDLE(svn_client__ra_session_from_path2(&session, nullptr,
                                                 path, svn_dirent_dirname(path, pool.Handle),
                                                 &rev, &rev,
                                                 CtxHandle,
                                                 pool.Handle));

    svn_revnum_t latest_revision;
    SVN_HANDLE(svn_ra_get_latest_revnum(session, &latest_revision, pool.Handle));

    revision = latest_revision;
    return true;
}

bool SvnClient::Youngest(Uri^ target, [Out] __int64 %revision)
{
    if (!target)
        throw gcnew ArgumentNullException("target");

    return Youngest(target, gcnew SvnYoungestArgs(), revision);
}

bool SvnClient::Youngest(Uri^ target, SvnYoungestArgs ^args, [Out] __int64 %revision)
{
    if (!target)
        throw gcnew ArgumentNullException("target");

    revision = SVN_INVALID_REVNUM;

    EnsureState(SvnContextState::AuthorizationInitialized);
    AprPool pool(%_pool);
    ArgsStore store(this, args, %pool);

    svn_ra_session_t *session;

    SVN_HANDLE(svn_client_open_ra_session2(&session,
                                           pool.AllocUri(target),
                                           nullptr, CtxHandle,
                                           pool.Handle, pool.Handle));

    svn_revnum_t latest_revision;
    SVN_HANDLE(svn_ra_get_latest_revnum(session, &latest_revision, pool.Handle));

    revision = latest_revision;
    return true;
}
