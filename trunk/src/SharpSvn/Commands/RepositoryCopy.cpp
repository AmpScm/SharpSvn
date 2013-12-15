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

bool SvnMultiCommandClient::Copy(String ^fromPath, String ^toPath)
{
    if (String::IsNullOrEmpty(fromPath) || ! IsValidRelpath(fromPath))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "fromPath");
    else if (String::IsNullOrEmpty(toPath) || ! IsValidRelpath(toPath))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "toPath");

    return Copy(fromPath, SVN_INVALID_REVNUM, toPath);
}

bool SvnMultiCommandClient::Copy(String ^fromPath, __int64 fromRev, String ^toPath)
{
    if (String::IsNullOrEmpty(fromPath) || ! IsValidRelpath(fromPath))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "fromPath");
    else if (String::IsNullOrEmpty(toPath) || ! IsValidRelpath(toPath))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "toPath");

    return Copy(fromPath, fromRev, toPath, gcnew SvnRepositoryCopyArgs());
}

bool SvnMultiCommandClient::Copy(String ^fromPath, String ^toPath, SvnRepositoryCopyArgs ^args)
{
    if (String::IsNullOrEmpty(fromPath) || ! IsValidRelpath(fromPath))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "fromPath");
    else if (String::IsNullOrEmpty(toPath) || ! IsValidRelpath(toPath))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "toPath");
    else if (! args)
        throw gcnew ArgumentNullException("args");

    return Copy(fromPath, SVN_INVALID_REVNUM, toPath, args);
}

bool SvnMultiCommandClient::Copy(String ^fromPath, __int64 fromRev, String ^toPath, SvnRepositoryCopyArgs ^args)
{
    if (String::IsNullOrEmpty(fromPath) || ! IsValidRelpath(fromPath))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "fromPath");
    else if (String::IsNullOrEmpty(toPath) || ! IsValidRelpath(toPath))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "toPath");
    else if (! args)
        throw gcnew ArgumentNullException("args");

    AprPool pool(%_pool);
    McArgsStore store(this, args);

    SVN_HANDLE(svn_client_mtcc_add_copy(pool.AllocRelpath(fromPath),
                                        SVN_IS_VALID_REVNUM(fromRev)
                                            ? (svn_revnum_t)fromRev
                                            : SVN_INVALID_REVNUM,
                                        pool.AllocRelpath(toPath),
                                        _mtcc,
                                        pool.Handle));

    return true;
}
