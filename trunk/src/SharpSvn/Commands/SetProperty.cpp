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

#include "Args/SetProperty.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;


[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.#SetProperty(System.String,System.String,System.String)")];
[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.#SetProperty(System.String,System.String,System.Collections.Generic.ICollection`1<System.Byte>,SharpSvn.SvnSetPropertyArgs)")];
[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.#SetProperty(System.String,System.String,System.String,SharpSvn.SvnSetPropertyArgs)")];
[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.#DeleteProperty(System.String,System.String)")];
[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.#DeleteProperty(System.String,System.String,SharpSvn.SvnSetPropertyArgs)")];
[module: SuppressMessage("Microsoft.Design", "CA1057:StringUriOverloadsCallSystemUriOverloads", Scope="member", Target="SharpSvn.SvnClient.#SetProperty(System.String,System.String,System.Collections.Generic.ICollection`1<System.Byte>)")];

bool SvnClient::SetProperty(String^ target, String^ propertyName, String^ value)
{
    if (String::IsNullOrEmpty(target))
        throw gcnew ArgumentNullException("target");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");
    else if (!value)
        throw gcnew ArgumentNullException("value");
    else if (!IsNotUri(target))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");

    return SetProperty(target, propertyName, value, gcnew SvnSetPropertyArgs());
}

bool SvnClient::RemoteSetProperty(Uri^ target, String^ propertyName, String^ value)
{
    if (!target)
        throw gcnew ArgumentNullException("target");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");
    else if (!value)
        throw gcnew ArgumentNullException("value");

    return RemoteSetProperty(target, propertyName, value, gcnew SvnSetPropertyArgs());
}

bool SvnClient::SetProperty(String^ target, String^ propertyName, ICollection<Byte>^ bytes)
{
    if (String::IsNullOrEmpty(target))
        throw gcnew ArgumentNullException("target");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");
    else if (!IsNotUri(target))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");
    else if (!bytes)
        throw gcnew ArgumentNullException("bytes");

    return SetProperty(target, propertyName, bytes, gcnew SvnSetPropertyArgs());
}

bool SvnClient::RemoteSetProperty(Uri^ target, String^ propertyName, ICollection<Byte>^ bytes)
{
    if (!target)
        throw gcnew ArgumentNullException("target");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");
    else if (!bytes)
        throw gcnew ArgumentNullException("bytes");

    return RemoteSetProperty(target, propertyName, bytes, gcnew SvnSetPropertyArgs());
}

bool SvnClient::SetProperty(String^ target, String^ propertyName, String^ value, SvnSetPropertyArgs^ args)
{
    if (String::IsNullOrEmpty(target))
        throw gcnew ArgumentNullException("target");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");
    else if (!IsNotUri(target))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");
    else if (!args)
        throw gcnew ArgumentNullException("args");
    else if (!value)
        throw gcnew ArgumentNullException("value");

    AprPool pool(%_pool);

    return InternalSetProperty(
        target,
        propertyName,
        pool.AllocPropertyValue(value, propertyName),
        args,
        %pool);
}

bool SvnClient::RemoteSetProperty(Uri^ target, String^ propertyName, String^ value, SvnSetPropertyArgs^ args)
{
    if (!target)
        throw gcnew ArgumentNullException("target");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");
    else if (!args)
        throw gcnew ArgumentNullException("args");
    else if (!value)
        throw gcnew ArgumentNullException("value");

    AprPool pool(%_pool);

    return InternalSetProperty(
        target,
        propertyName,
        pool.AllocPropertyValue(value, propertyName),
        args,
        %pool);
}

bool SvnClient::SetProperty(String^ target, String^ propertyName, ICollection<Byte>^ bytes, SvnSetPropertyArgs^ args)
{
    if (String::IsNullOrEmpty(target))
        throw gcnew ArgumentNullException("target");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");
    else if (!IsNotUri(target))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");
    else if (!args)
        throw gcnew ArgumentNullException("args");
    else if (!bytes)
        throw gcnew ArgumentNullException("bytes");

    AprPool pool(%_pool);

    array<Byte> ^byteArray = dynamic_cast<array<Byte>^>(bytes);

    if (!byteArray)
    {
        byteArray = gcnew array<Byte>(bytes->Count);

        bytes->CopyTo(byteArray, 0);
    }

    return InternalSetProperty(target, propertyName, pool.AllocSvnString(byteArray), args, %pool);
}

bool SvnClient::RemoteSetProperty(Uri^ target, String^ propertyName, ICollection<Byte>^ bytes, SvnSetPropertyArgs^ args)
{
    if (!target)
        throw gcnew ArgumentNullException("target");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");
    else if (!args)
        throw gcnew ArgumentNullException("args");
    else if (!bytes)
        throw gcnew ArgumentNullException("bytes");

    AprPool pool(%_pool);

    array<Byte> ^byteArray = dynamic_cast<array<Byte>^>(bytes);

    if (!byteArray)
    {
        byteArray = gcnew array<Byte>(bytes->Count);

        bytes->CopyTo(byteArray, 0);
    }

    return InternalSetProperty(target, propertyName, pool.AllocSvnString(byteArray), args, %pool);
}

bool SvnClient::DeleteProperty(String^ target, String^ propertyName)
{
    if (String::IsNullOrEmpty(target))
        throw gcnew ArgumentNullException("target");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");
    else if (!IsNotUri(target))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");

    return DeleteProperty(target, propertyName, gcnew SvnSetPropertyArgs());
}

bool SvnClient::RemoteDeleteProperty(Uri^ target, String^ propertyName)
{
    if (!target)
        throw gcnew ArgumentNullException("target");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");

    return RemoteDeleteProperty(target, propertyName, gcnew SvnSetPropertyArgs());
}

bool SvnClient::DeleteProperty(String^ target, String^ propertyName, SvnSetPropertyArgs^ args)
{
    if (String::IsNullOrEmpty(target))
        throw gcnew ArgumentNullException("target");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");
    else if (!args)
        throw gcnew ArgumentNullException("args");
    else if (!IsNotUri(target))
        throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");

    AprPool pool(%_pool);

    return InternalSetProperty(target, propertyName, nullptr, args, %pool);
}

bool SvnClient::RemoteDeleteProperty(Uri^ target, String^ propertyName, SvnSetPropertyArgs^ args)
{
    if (!target)
        throw gcnew ArgumentNullException("target");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");
    else if (!args)
        throw gcnew ArgumentNullException("args");

    AprPool pool(%_pool);

    return InternalSetProperty(target, propertyName, nullptr, args, %pool);
}

bool SvnClient::InternalSetProperty(String^ target, String^ propertyName, const svn_string_t* value, SvnSetPropertyArgs^ args, AprPool^ pool)
{
    if (!target)
        throw gcnew ArgumentNullException("target");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");

    EnsureState(SvnContextState::ConfigLoaded); // We might need repository access
    ArgsStore store(this, args, pool);

    const char* pcPropertyName = pool->AllocString(propertyName);

    if (!svn_prop_name_is_valid(pcPropertyName))
        throw gcnew ArgumentException(SharpSvnStrings::PropertyNameIsNotValid, "propertyName");

    svn_error_t *r = svn_client_propset_local(
                pcPropertyName,
                value,
    AllocDirentArray(NewSingleItemCollection(SvnTools::GetNormalizedFullPath(target)), pool),
        (svn_depth_t)args->Depth,
        args->SkipChecks,
        CreateChangeListsList(args->ChangeLists, pool), // Intersect ChangeLists
        CtxHandle,
        pool->Handle);

    return args->HandleResult(this, r, target);
}

bool SvnClient::InternalSetProperty(Uri^ target, String^ propertyName, const svn_string_t* value, SvnSetPropertyArgs^ args, AprPool^ pool)
{
    if (!target)
        throw gcnew ArgumentNullException("target");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");
    else if (String::IsNullOrEmpty(propertyName))
        throw gcnew ArgumentNullException("propertyName");

    EnsureState(SvnContextState::AuthorizationInitialized); // We might need repository access
    ArgsStore store(this, args, pool);
    CommitResultReceiver crr(this);

    const char* pcPropertyName = pool->AllocString(propertyName);

    if (!svn_prop_name_is_valid(pcPropertyName))
        throw gcnew ArgumentException(SharpSvnStrings::PropertyNameIsNotValid, "propertyName");

    svn_error_t *r = svn_client_propset_remote(
        pcPropertyName,
        value,
        pool->AllocUri(target),
        args->SkipChecks,
        (svn_revnum_t)args->BaseRevision,
        CreateRevPropList(args->LogProperties, pool),
        crr.CommitCallback, crr.CommitBaton,
        CtxHandle,
        pool->Handle);

    return args->HandleResult(this, r, target);
}}
