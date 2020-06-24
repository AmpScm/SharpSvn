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

bool SvnMultiCommandClient::SetProperty(String^ path, String^ propertyName, String^ value)
{
    if (! IsValidRelpath(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "path");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");

    return SetProperty(path, propertyName, value, gcnew SvnRepositorySetPropertyArgs());
}

bool SvnMultiCommandClient::SetProperty(String^ path, String^ propertyName, ICollection<Byte>^ bytes)
{
    if (! IsValidRelpath(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "path");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");

    return SetProperty(path, propertyName, bytes, gcnew SvnRepositorySetPropertyArgs());
}

bool SvnMultiCommandClient::DeleteProperty(String^ path, String^ propertyName)
{
    if (! IsValidRelpath(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "path");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");

    return DeleteProperty(path, propertyName, gcnew SvnRepositorySetPropertyArgs());
}

bool SvnMultiCommandClient::SetProperty(String^ path, String^ propertyName, String^ value, SvnRepositorySetPropertyArgs ^args)
{
    if (! IsValidRelpath(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "path");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");
    else if (! args)
        throw gcnew ArgumentNullException("args");

    AprPool pool(%_pool);

    return InternalSetProperty(path, propertyName, pool.AllocSvnString(value), args, %pool);
}

bool SvnMultiCommandClient::SetProperty(String^ path, String^ propertyName, ICollection<Byte>^ bytes, SvnRepositorySetPropertyArgs ^args)
{
    if (! IsValidRelpath(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "path");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");
    else if (! args)
        throw gcnew ArgumentNullException("args");

    AprPool pool(%_pool);

    array<Byte> ^byteArray = dynamic_cast<array<Byte>^>(bytes);

    if (!byteArray)
    {
        byteArray = gcnew array<Byte>(bytes->Count);

        bytes->CopyTo(byteArray, 0);
    }

    return InternalSetProperty(path, propertyName, pool.AllocSvnString(byteArray), args, %pool);
}

bool SvnMultiCommandClient::DeleteProperty(String^ path, String^ propertyName, SvnRepositorySetPropertyArgs ^args)
{
    if (! IsValidRelpath(path))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRelativePath, "path");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");
    else if (! args)
        throw gcnew ArgumentNullException("args");

    AprPool pool(%_pool);

    return InternalSetProperty(path, propertyName, nullptr, args, %pool);
}

bool SvnMultiCommandClient::InternalSetProperty(String^ path, String^ propertyName, const svn_string_t* value, SvnRepositorySetPropertyArgs^ args, AprPool^ pool)
{
    McArgsStore store(this, args);

    SVN_HANDLE(svn_client_mtcc_add_propset(pool->AllocRelpath(path),
                                           pool->AllocString(propertyName),
                                           value,
                                           FALSE /* skip_checks */,
                                           _mtcc,
                                           pool->Handle));

    return true;
}
