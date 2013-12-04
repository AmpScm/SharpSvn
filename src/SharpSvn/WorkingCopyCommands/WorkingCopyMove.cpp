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

#include "WorkingCopyArgs/WorkingCopyMove.h"

#include <private/svn_wc_private.h>

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnWorkingCopyClient::Move(String^ sourcePath, String^ toPath)
{
	if (String::IsNullOrEmpty(sourcePath))
		throw gcnew ArgumentNullException("sourcePath");
	else if (String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");

	return Move(sourcePath, toPath, gcnew SvnWorkingCopyMoveArgs());
}

bool SvnWorkingCopyClient::Move(String^ sourcePath, String^ toPath, SvnWorkingCopyMoveArgs^ args)
{
	if (String::IsNullOrEmpty(sourcePath))
		throw gcnew ArgumentNullException("sourcePath");
	else if (String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

    svn_error_t *r = SVN_NO_ERROR;
    const char *lock1;
    const char *lock2;

    const char *pSrc = pool.AllocAbsoluteDirent(sourcePath);
    const char *pDst = pool.AllocAbsoluteDirent(toPath);

    SVN_HANDLE(svn_client_get_wc_root(&lock1, pSrc, CtxHandle, pool.Handle, pool.Handle));
    SVN_HANDLE(svn_client_get_wc_root(&lock2, pDst, CtxHandle, pool.Handle, pool.Handle));

    if (!strcmp(lock1, lock2))
    {
        // The simple case: Everything in one WC.
        lock1 = svn_dirent_get_longest_ancestor(pSrc, pDst, pool.Handle);
        SVN_HANDLE(svn_wc__acquire_write_lock(&lock1, CtxHandle->wc_ctx, lock1, FALSE, pool.Handle, pool.Handle));
        lock2 = NULL;
    }
    else
    {
        if (strcmp(lock1, pSrc) != 0)
            lock1 = svn_dirent_dirname(pSrc, pool.Handle);
        else
            lock1 = pSrc;
        if (strcmp(lock2, pDst) != 0)
            lock2 = svn_dirent_dirname(pDst, pool.Handle);
        else
            lock2 = pDst;

        SVN_HANDLE(svn_wc__acquire_write_lock(&lock1, CtxHandle->wc_ctx, lock1, FALSE, pool.Handle, pool.Handle));

        r = svn_wc__acquire_write_lock(&lock2, CtxHandle->wc_ctx, lock2, FALSE, pool.Handle, pool.Handle);

        if (r)
        {
            SVN_HANDLE(svn_error_compose_create(r, svn_wc__release_write_lock(CtxHandle->wc_ctx, lock1, pool.Handle)));
        }
    }

    r = svn_wc_move(
		CtxHandle->wc_ctx,
		pSrc,
		pDst,
		args->MetaDataOnly,
		CtxHandle->cancel_func, CtxHandle->cancel_baton,
		CtxHandle->notify_func2, CtxHandle->notify_baton2,
		pool.Handle);

    if (lock1)
        r = svn_error_compose_create(r, svn_wc__release_write_lock(CtxHandle->wc_ctx, lock1, pool.Handle));
    if (lock2)
        r = svn_error_compose_create(r, svn_wc__release_write_lock(CtxHandle->wc_ctx, lock2, pool.Handle));

	return args->HandleResult(this, r, sourcePath);
}
