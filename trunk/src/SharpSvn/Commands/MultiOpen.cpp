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

#include "SvnMultiCommandClient.h"
#include "Args/SvnRepositoryOperationArgs.h"

using namespace SharpSvn;

bool SvnMultiCommandClient::Open(Uri ^sessionUri)
{
    if (! sessionUri)
        throw gcnew ArgumentNullException("sessionUri");
    else if (! SvnBase::IsValidReposUri(sessionUri))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "sessionUri");

    return SvnMultiCommandClient::Open(sessionUri, gcnew SvnRepositoryOperationArgs());
}

bool SvnMultiCommandClient::Open(Uri ^sessionUri, SvnRepositoryOperationArgs ^args)
{
    if (! sessionUri)
        throw gcnew ArgumentNullException("sessionUri");
    else if (! args)
        throw gcnew ArgumentNullException("args");
    else if (! SvnBase::IsValidReposUri(sessionUri))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "sessionUri");

    if (_mtcc || !_pool.Handle)
        throw gcnew InvalidOperationException();

    EnsureState(SvnContextState::AuthorizationInitialized);
    AprPool pool(%_pool);
    ArgsStore store(this, args, %pool);

    svn_client_mtcc_t *mtcc;

    SVN_HANDLE(svn_client_mtcc_create(&mtcc,
                                      pool.AllocUri(sessionUri),
                                      (svn_revnum_t)args->BaseRevision,
                                      CtxHandle,
                                      _pool.Handle,
                                      pool.Handle));

    _mtcc = mtcc;
    _roArgs = args;

    return true;
}

bool SvnMultiCommandClient::Commit()
{
    SvnCommitResult ^result;

    return Commit(result);
}

bool SvnMultiCommandClient::Commit([Out] SvnCommitResult ^%result)
{
    if (IsDisposed || !_mtcc || !_pool.Handle)
        throw gcnew InvalidOperationException();

    SvnRepositoryOperationArgs ^args = _roArgs;

    if (! args)
        args = gcnew SvnRepositoryOperationArgs();

    EnsureState(SvnContextState::AuthorizationInitialized);
    try
    {
        AprPool pool(%_pool);
        ArgsStore store(this, args, %pool);
        CommitResultReceiver crr(this);

        result = nullptr;
        svn_error_t *err;

        err = svn_client_mtcc_commit(CreateRevPropList(args->LogProperties, %pool),
                                     crr.CommitCallback, crr.CommitBaton,
                                     _mtcc,
                                     pool.Handle);

        result = crr.CommitResult;

        return args->HandleResult(this, err);
    }
    finally
    {
        _mtcc = nullptr;
        _pool.~AprPool(); // Kills the SvnMultiCommandClient
    }
}