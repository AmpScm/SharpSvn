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

SvnMultiCommandClient::SvnMultiCommandClient(SvnClient ^via)
    : _pool(via->ClientPool), SvnClientContext(%_pool, via)
{
}

SvnMultiCommandClient::SvnMultiCommandClient()
    : _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
}

SvnMultiCommandClient::SvnMultiCommandClient(Uri ^sessionUri)
    : _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
    Open(sessionUri);
}

SvnMultiCommandClient::SvnMultiCommandClient(Uri ^sessionUri, SvnRepositoryOperationArgs ^args)
    : _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
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
