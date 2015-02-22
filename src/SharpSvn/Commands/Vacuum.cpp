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

#include "Args/Vacuum.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::Vacuum(String ^path)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");

    return Vacuum(path, gcnew SvnVacuumArgs());
}

bool SvnClient::Vacuum(String ^path, SvnVacuumArgs^ args)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");
    else if (!args)
        throw gcnew ArgumentNullException("args");

    EnsureState(SvnContextState::ConfigLoaded);
    AprPool pool(%_pool);
    ArgsStore store(this, args, %pool);

    svn_error_t *r = svn_client_vacuum(
                      pool.AllocAbsoluteDirent(path),
                      TRUE /* remove_unversioned_items */,
                      TRUE /* remove_ignored_items */,
                      TRUE /* fix_recorded_timestamps*/,
                      FALSE /* vacuum_pristines */,
                      args->IncludeExternals,
                      CtxHandle,
                      pool.Handle);

    return args->HandleResult(this, r, path);
}
