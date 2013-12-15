// Copyright 2007-2008 The SharpSvn Project
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

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types


using namespace SharpSvn;
using namespace SharpSvn::Implementation;



[module: SuppressMessage("Microsoft.Naming", "CA1709:IdentifiersShouldBeCasedCorrectly", Scope="member", Target="SharpSvn.SvnRepositoryFileSystem.#FsFs", MessageId="Fs")];

void
svnrepository_notify_func(void *baton,
                          const svn_repos_notify_t *notify,
                          apr_pool_t *scratch_pool)
{
    SvnRepositoryClient^ client = AprBaton<SvnRepositoryClient^>::Get(baton);

    AprPool tmpPool(scratch_pool, false);
    SvnRepositoryNotifyEventArgs^ ea = gcnew SvnRepositoryNotifyEventArgs(notify, %tmpPool);

    try
    {
        client->HandleClientNotify(ea);
    }
    finally
    {
        ea->Detach(false);
    }
}

static SvnRepositoryClient::SvnRepositoryClient()
{
    repos_notify_func = svnrepository_notify_func;
}

SvnRepositoryClient::SvnRepositoryClient()
: _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
    _clientBaton = gcnew AprBaton<SvnRepositoryClient^>(this);
}

SvnRepositoryClient::~SvnRepositoryClient()
{
    delete _clientBaton;
}

String^ SvnRepositoryClient::FindRepositoryRoot(Uri^ repositoryUri)
{
    if (!repositoryUri)
        throw gcnew ArgumentNullException("repositoryUri");

    EnsureState(SvnContextState::ConfigLoaded);

    AprPool pool(%_pool);

    const char* root = svn_repos_find_root_path(
        pool.AllocUri(repositoryUri),
        pool.Handle);

    return root ? Utf8_PtrToString(root) : nullptr;
}

void SvnRepositoryClient::HandleClientCancel(SvnCancelEventArgs^ e)
{
    __super::HandleClientCancel(e);

    if (! e->Cancel)
        OnCancel(e);
}

void SvnRepositoryClient::OnCancel(SvnCancelEventArgs^ e)
{
    Cancel(this, e);
}

void SvnRepositoryClient::HandleClientNotify(SvnRepositoryNotifyEventArgs^ e)
{
    if (CurrentCommandArgs)
        CurrentCommandArgs->RaiseOnNotify(e);

    OnNotify(e);
}

void SvnRepositoryClient::OnNotify(SvnRepositoryNotifyEventArgs^ e)
{
    Notify(this, e);
}
