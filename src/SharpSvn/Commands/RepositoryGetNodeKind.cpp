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

using namespace SharpSvn;

bool SvnMultiCommandClient::GetNodeKind(String ^path, SvnNodeKind %kind)
{
    if (! IsValidRelpath(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "path");

    return GetNodeKind(path, gcnew SvnRepositoryGetNodeKindArgs(), kind);
}

bool SvnMultiCommandClient::GetNodeKind(String ^path, SvnRepositoryGetNodeKindArgs ^args, SvnNodeKind %kind)
{
    if (! IsValidRelpath(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "path");
    else if (! args)
        throw gcnew ArgumentNullException("args");

    AprPool pool(%_pool);
    ArgsStore store(this, args);

    svn_node_kind_t node_kind;

    SVN_HANDLE(svn_client_mtcc_check_path(&node_kind,
                                          pool.AllocRelpath(path),
                                          FALSE /* check_repository */,
                                          _mtcc,
                                          pool.Handle));

    kind = (SvnNodeKind)node_kind;

    return true;
}
