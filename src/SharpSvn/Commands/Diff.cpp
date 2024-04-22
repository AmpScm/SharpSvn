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

#include "SvnStreamWrapper.h"
#include "Args/Diff.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;
using System::IO::File;
using System::IO::FileMode;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Diff(SharpSvn.SvnTarget,SharpSvn.SvnRevision,SharpSvn.SvnRevision,SharpSvn.SvnDiffArgs,System.IO.FileStream&):System.Boolean", MessageId="4#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Diff(SharpSvn.SvnTarget,SharpSvn.SvnRevision,SharpSvn.SvnRevision,System.IO.FileStream&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Diff(SharpSvn.SvnTarget,SharpSvn.SvnTarget,SharpSvn.SvnDiffArgs,System.IO.FileStream&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Diff(SharpSvn.SvnTarget,SharpSvn.SvnTarget,System.IO.FileStream&):System.Boolean", MessageId="2#")];

bool SvnClient::Diff(SvnTarget^ from, SvnTarget^ to, Stream^ result)
{
    if (!from)
        throw gcnew ArgumentNullException("from");
    else if (!to)
        throw gcnew ArgumentNullException("to");
    else if (!result)
        throw gcnew ArgumentNullException("result");

    return Diff(from, to, gcnew SvnDiffArgs(), result);
}

bool SvnClient::Diff(SvnTarget^ from, SvnTarget^ to, SvnDiffArgs^ args, Stream^ result)
{
    if (!from)
        throw gcnew ArgumentNullException("from");
    else if (!to)
        throw gcnew ArgumentNullException("to");
    else if (!args)
        throw gcnew ArgumentNullException("args");
    else if (!result)
        throw gcnew ArgumentNullException("result");

    EnsureState(SvnContextState::AuthorizationInitialized);
    AprPool pool(%_pool);

    svn_config_t* cfg = (svn_config_t*)apr_hash_get(CtxHandle->config, SVN_CONFIG_CATEGORY_CONFIG, APR_HASH_KEY_STRING);

    const char* diff_cmd;
    svn_config_get(cfg, &diff_cmd, SVN_CONFIG_SECTION_HELPERS, SVN_CONFIG_OPTION_DIFF_CMD, nullptr);

    // Create an instance of AprStreamFile if the diff_cmd is specified or
    // SvnStreamWrapper if diff_cmd is null. External diff tool (specified via
    // diff_cmd) supports only file stream, while libsvn_diff supports both
    // (file stream and SvnStream).
    //
    // Reference: https://github.com/apache/subversion/blob/1.14.3/subversion/libsvn_client/diff.c#L1010
    if (diff_cmd) {
        AprStreamFile out(result, %pool);
        AprStreamFile err(args->ErrorStream ? args->ErrorStream : System::IO::Stream::Null, %pool);

        svn_stream_t *outstream = svn_stream_from_aprfile2(out.CreateHandle(), TRUE, pool.Handle);
        svn_stream_t* errstream = svn_stream_from_aprfile2(err.CreateHandle(), TRUE, pool.Handle);

        return InternalDiff(from, to, args, outstream, errstream, %pool);
    }
    else {
        SvnStreamWrapper out(result, false, true, %pool);
        SvnStreamWrapper err(args->ErrorStream ? args->ErrorStream : System::IO::Stream::Null, false, true, %pool);

        return InternalDiff(from, to, args, out.Handle, err.Handle, %pool);
    }
}

bool SvnClient::InternalDiff(SvnTarget^ from, SvnTarget^ to, SvnDiffArgs^ args, svn_stream_t *outstream, svn_stream_t *errstream, AprPool^ pool) {
    ICollection<String^>^ diffArgs = args->DiffArguments;

    ArgsStore store(this, args, pool);

    if (!diffArgs)
        diffArgs = safe_cast<IList<String^>^>(gcnew array<String^>(0));

    svn_error_t *r = svn_client_diff7(
        AllocArray(diffArgs, pool),
        from->AllocAsString(pool),
        from->GetSvnRevision(SvnRevision::Working, SvnRevision::Head)->AllocSvnRevision(pool),
        to->AllocAsString(pool),
        to->GetSvnRevision(SvnRevision::Working, SvnRevision::Head)->AllocSvnRevision(pool),
        args->RelativeToPath ? pool->AllocDirent(args->RelativeToPath) : nullptr,
        (svn_depth_t)args->Depth,
        args->IgnoreAncestry,
        args->NoAdded,
        args->NoDeleted,
        args->CopiesAsAdds,
        args->IgnoreContentType,
        args->NoProperties,
        args->PropertiesOnly,
        args->UseGitFormat,
        args->PrettyPrintMergeInfo,
        pool->AllocString(args->HeaderEncoding),
        outstream,
        errstream,
        CreateChangeListsList(args->ChangeLists, pool), // Intersect ChangeLists
        CtxHandle,
        pool->Handle);

    return args->HandleResult(this, r, from);
}

bool SvnClient::Diff(SvnTarget^ source, SvnRevisionRange^ range, Stream^ result)
{
    if (!source)
        throw gcnew ArgumentNullException("source");
    else if (!range)
        throw gcnew ArgumentNullException("range");
    else if (!result)
        throw gcnew ArgumentNullException("result");

    return Diff(source, range, gcnew SvnDiffArgs(), result);
}

bool SvnClient::Diff(SvnTarget^ source, SvnRevisionRange^ range, SvnDiffArgs^ args, Stream^ result)
{
    if (!source)
        throw gcnew ArgumentNullException("source");
    else if (!range)
        throw gcnew ArgumentNullException("range");
    else if (!result)
        throw gcnew ArgumentNullException("result");
    else if (!args)
        throw gcnew ArgumentNullException("args");

    EnsureState(SvnContextState::AuthorizationInitialized);
    AprPool pool(%_pool);

    svn_config_t* cfg = (svn_config_t*)apr_hash_get(CtxHandle->config, SVN_CONFIG_CATEGORY_CONFIG, APR_HASH_KEY_STRING);

    const char* diff_cmd;
    svn_config_get(cfg, &diff_cmd, SVN_CONFIG_SECTION_HELPERS, SVN_CONFIG_OPTION_DIFF_CMD, nullptr);

    // Create an instance of AprStreamFile if the diff_cmd is specified, or
    // SvnStreamWrapper if diff_cmd is null. The external diff tool (specified
    // via diff_cmd) supports only file streams, while libsvn_diff supports
    // both (file stream and SvnStream).
    //
    // Reference: https://github.com/apache/subversion/blob/1.14.3/subversion/libsvn_client/diff.c#L1010
    if (diff_cmd) {
        AprStreamFile out(result, %pool);
        AprStreamFile err(args->ErrorStream ? args->ErrorStream : System::IO::Stream::Null, %pool);

        svn_stream_t* outstream = svn_stream_from_aprfile2(out.CreateHandle(), TRUE, pool.Handle);
        svn_stream_t* errstream = svn_stream_from_aprfile2(err.CreateHandle(), TRUE, pool.Handle);

        return InternalDiff(source, range, args, outstream, errstream, %pool);
    }
    else {
        SvnStreamWrapper out(result, false, true, %pool);
        SvnStreamWrapper err(args->ErrorStream ? args->ErrorStream : System::IO::Stream::Null, false, true, %pool);

        return InternalDiff(source, range, args, out.Handle, err.Handle, %pool);
    }
}

bool SvnClient::InternalDiff(SvnTarget^ source, SvnRevisionRange^ range, SvnDiffArgs^ args, svn_stream_t* outstream, svn_stream_t* errstream, AprPool^ pool) {
    ICollection<String^>^ diffArgs = args->DiffArguments;

    svn_opt_revision_t pegRev = source->Revision->ToSvnRevision();
    svn_opt_revision_t fromRev = range->StartRevision->ToSvnRevision();
    svn_opt_revision_t toRev = range->EndRevision->ToSvnRevision();

    ArgsStore store(this, args, pool);

    if (!diffArgs)
        diffArgs = safe_cast<IList<String^>^>(gcnew array<String^>(0));

    svn_error_t *r = svn_client_diff_peg7(
        AllocArray(diffArgs, pool),
        source->AllocAsString(pool),
        &pegRev,
        &fromRev,
        &toRev,
        args->RelativeToPath ? pool->AllocDirent(args->RelativeToPath) : nullptr,
        (svn_depth_t)args->Depth,
        args->IgnoreAncestry,
        args->NoAdded,
        args->NoDeleted,
        args->CopiesAsAdds,
        args->IgnoreContentType,
        args->NoProperties,
        args->PropertiesOnly,
        args->UseGitFormat,
        args->PrettyPrintMergeInfo,
        pool->AllocString(args->HeaderEncoding),
        outstream,
        errstream,
        CreateChangeListsList(args->ChangeLists, pool), // Intersect ChangeLists
        CtxHandle,
        pool->Handle);

    return args->HandleResult(this, r, source);
}
