// Copyright 2013 The SharpSvn Project
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
#include "SvnStreamWrapper.h"

using namespace SharpSvn;
using SharpSvn::Implementation::SvnStreamWrapper;

bool SvnMultiCommandClient::UpdateFile(String ^path, System::IO::Stream ^newData)
{
    if (! IsValidRelpath(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "path");
    else if (! newData)
        throw gcnew ArgumentNullException("newData");

    return UpdateFile(path, newData, gcnew SvnRepositoryUpdateFileArgs());
}

bool SvnMultiCommandClient::UpdateFile(String ^path, System::IO::Stream ^newData, SvnRepositoryUpdateFileArgs ^args)
{
    if (! IsValidRelpath(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "path");
    else if (! newData)
        throw gcnew ArgumentNullException("newData");
    else if (! args)
        throw gcnew ArgumentNullException("args");

    if (! IsValidRelpath(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "path");
    else if (! args)
        throw gcnew ArgumentNullException("args");

    AprPool pool(%_pool);
    ArgsStore store(this, args);

    SvnStreamWrapper ^wp = gcnew SvnStreamWrapper(newData, true, false, %_pool);

    if (!_refs)
        _refs = gcnew List<System::IDisposable^>();

    _refs->Add(wp);

    SVN_HANDLE(svn_client_mtcc_add_update_file(pool.AllocRelpath(path),
                                               wp->Handle,
                                               nullptr /* checksum */,
                                               nullptr /* base_stream */,
                                               nullptr /* base_stream_checksum */,
                                               _mtcc,
                                               pool.Handle));

    return true;
}