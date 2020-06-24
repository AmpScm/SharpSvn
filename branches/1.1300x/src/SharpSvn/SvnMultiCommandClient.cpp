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

#include "UnmanagedStructs.h"

using namespace SharpSvn;

struct SvnMultiCommandClientCallBacks
{
    static void __cdecl svn_wc_notify_func2(void *baton, const svn_wc_notify_t *notify, apr_pool_t *pool);
};

void SvnMultiCommandClient::Initialize()
{
    void* baton = (void*)_clientBaton->Handle;

    CtxHandle->notify_baton2 = baton;
    CtxHandle->notify_func2 = &SvnMultiCommandClientCallBacks::svn_wc_notify_func2;
}

SvnMultiCommandClient::~SvnMultiCommandClient()
{
    if (_clientBaton)
        delete _clientBaton;

    if (_refs)
        _refs = nullptr;
}

SvnMultiCommandClient::SvnMultiCommandClient(SvnClient ^via)
    : _pool(via->ClientPool), SvnClientContext(%_pool, via)
{
    //_clientBaton = nullptr;
    // No call to Initialize() as we just wrap the SvnClient
}

SvnMultiCommandClient::SvnMultiCommandClient()
    : _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
    _clientBaton = gcnew AprBaton<SvnMultiCommandClient^>(this);

    Initialize();
}

SvnMultiCommandClient::SvnMultiCommandClient(Uri ^sessionUri)
    : _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
    _clientBaton = gcnew AprBaton<SvnMultiCommandClient^>(this);

    Initialize();

    Open(sessionUri);
}

SvnMultiCommandClient::SvnMultiCommandClient(Uri ^sessionUri, SvnRepositoryOperationArgs ^args)
    : _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
    _clientBaton = gcnew AprBaton<SvnMultiCommandClient^>(this);

    Initialize();

    if (!Open(sessionUri, args))
        throw (args->LastException ? static_cast<Exception^>(args->LastException)
                                   : gcnew InvalidOperationException());
}

bool SvnClient::RepositoryOperation(Uri ^anchor, SvnRepositoryOperationBuilder ^builder)
{
    if (! anchor)
        throw gcnew ArgumentNullException("anchor");
    else if (! builder)
        throw gcnew ArgumentNullException("builder");
    else if (! SvnBase::IsValidReposUri(anchor))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "anchor");

    return RepositoryOperation(anchor, gcnew SvnRepositoryOperationArgs(), builder);
}

bool SvnClient::RepositoryOperation(Uri ^anchor, SvnRepositoryOperationArgs ^args, SvnRepositoryOperationBuilder ^builder)
{
    if (! anchor)
        throw gcnew ArgumentNullException("anchor");
    else if (! args)
        throw gcnew ArgumentNullException("args");
    else if (! builder)
        throw gcnew ArgumentNullException("builder");
    else if (! SvnBase::IsValidReposUri(anchor))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "anchor");

    SvnCommitResult ^result;

    return RepositoryOperation(anchor, args, builder, result);
}

bool SvnClient::RepositoryOperation(Uri ^anchor, SvnRepositoryOperationBuilder ^builder, [Out] SvnCommitResult ^%result)
{
    if (! anchor)
        throw gcnew ArgumentNullException("anchor");
    else if (! builder)
        throw gcnew ArgumentNullException("builder");
    else if (! SvnBase::IsValidReposUri(anchor))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "anchor");

    return RepositoryOperation(anchor, gcnew SvnRepositoryOperationArgs(), builder, result);
}

bool SvnClient::RepositoryOperation(Uri ^anchor, SvnRepositoryOperationArgs ^args, SvnRepositoryOperationBuilder ^builder, [Out] SvnCommitResult ^%result)
{
    if (! anchor)
        throw gcnew ArgumentNullException("anchor");
    else if (! args)
        throw gcnew ArgumentNullException("args");
    else if (! builder)
        throw gcnew ArgumentNullException("builder");
    else if (! SvnBase::IsValidReposUri(anchor))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "anchor");

    EnsureState(SvnContextState::AuthorizationInitialized);
    AprPool pool(%_pool);
    ArgsStore store(this, args, %pool);

    {
        // This is essentially using(SvnMultiCommandClient mucc = new mucc(this)
        // on block, so this handles all cleanup in case of errors
        SvnMultiCommandClient mucc(this); 

        if (!mucc.ViaOpen(anchor, args))
            return false;

        builder(%mucc);

        return mucc.ViaCommit(result);
    }
}



void SvnMultiCommandClientCallBacks::svn_wc_notify_func2(void *baton, const svn_wc_notify_t *notify, apr_pool_t *pool)
{
    SvnMultiCommandClient^ client = AprBaton<SvnMultiCommandClient^>::Get((IntPtr)baton);
    AprPool aprPool(pool, false);

    SvnNotifyEventArgs^ ea = gcnew SvnNotifyEventArgs(notify, client->CurrentCommandArgs->CommandType, %aprPool);

    try
    {
        client->HandleClientNotify(ea);
    }
    finally
    {
        ea->Detach(false);
    }
}

void SvnMultiCommandClient::OnCancel(SvnCancelEventArgs^ e)
{
    Cancel(this, e);
}

void SvnMultiCommandClient::OnProgress(SvnProgressEventArgs^ e)
{
    Progress(this, e);
}

void SvnMultiCommandClient::OnCommitting(SvnCommittingEventArgs^ e)
{
    Committing(this, e);
}

void SvnMultiCommandClient::OnCommitted(SvnCommittedEventArgs^ e)
{
    Committed(this, e);
}

void SvnMultiCommandClient::OnNotify(SvnNotifyEventArgs^ e)
{
    Notify(this, e);
}

void SvnMultiCommandClient::OnSvnError(SvnErrorEventArgs^ e)
{
    SvnError(this, e);
}

void SvnMultiCommandClient::OnProcessing(SvnProcessingEventArgs^ e)
{
    Processing(this, e);
}

void SvnMultiCommandClient::HandleClientCancel(SvnCancelEventArgs^ e)
{
    __super::HandleClientCancel(e);

    if (! e->Cancel)
        OnCancel(e);
}

void SvnMultiCommandClient::HandleClientProgress(SvnProgressEventArgs^ e)
{
    __super::HandleClientProgress(e);

    OnProgress(e);
}

void SvnMultiCommandClient::HandleClientCommitting(SvnCommittingEventArgs^ e)
{
    __super::HandleClientCommitting(e);

    if (! e->Cancel)
        OnCommitting(e);
}

void SvnMultiCommandClient::HandleClientCommitted(SvnCommittedEventArgs^ e)
{
    __super::HandleClientCommitted(e);

    OnCommitted(e);
}

void SvnMultiCommandClient::HandleClientNotify(SvnNotifyEventArgs^ e)
{
    __super::HandleClientNotify(e);

    OnNotify(e);
}

void SvnMultiCommandClient::HandleClientError(SvnErrorEventArgs^ e)
{
    __super::HandleClientError(e);

    OnSvnError(e);
}

void SvnMultiCommandClient::HandleProcessing(SvnProcessingEventArgs^ e)
{
    __super::HandleProcessing(e);
    OnProcessing(e);
}
