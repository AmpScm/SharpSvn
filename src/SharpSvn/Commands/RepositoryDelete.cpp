// Copyright 2007-2025 The SharpSvn Project
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

bool SvnMultiCommandClient::Delete(String ^path)
{
    if (! IsValidRelpath(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "path");

    return Delete(path, gcnew SvnRepositoryDeleteArgs());
}

bool SvnMultiCommandClient::Delete(String ^path, SvnRepositoryDeleteArgs ^args)
{
    if (! IsValidRelpath(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "path");
    else if (! args)
        throw gcnew ArgumentNullException("args");

    AprPool pool(%_pool);
    McArgsStore store(this, args);

    SVN_HANDLE(svn_client_mtcc_add_delete(pool.AllocRelpath(path),
                                          _mtcc,
                                          pool.Handle));

    return true;
}
