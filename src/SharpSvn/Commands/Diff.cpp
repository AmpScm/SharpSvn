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
#include "SvnAll.h"
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
	ArgsStore store(this, args, %pool);

	AprStreamFile out(result, %pool);
	AprStreamFile err(args->ErrorStream ? args->ErrorStream : gcnew System::IO::MemoryStream(), %pool);

	svn_stream_t *outstream = svn_stream_from_aprfile2(out.CreateHandle(), TRUE, pool.Handle);
	svn_stream_t *errstream = svn_stream_from_aprfile2(err.CreateHandle(), TRUE, pool.Handle);

	ICollection<String^>^ diffArgs = args->DiffArguments;

	if (!diffArgs)
		diffArgs = safe_cast<IList<String^>^>(gcnew array<String^>(0));

	svn_error_t *r = svn_client_diff6(
		AllocArray(diffArgs, %pool),
		from->AllocAsString(%pool),
		from->GetSvnRevision(SvnRevision::Working, SvnRevision::Head)->AllocSvnRevision(%pool),
		to->AllocAsString(%pool),
		to->GetSvnRevision(SvnRevision::Working, SvnRevision::Head)->AllocSvnRevision(%pool),
		args->RelativeToPath ? pool.AllocDirent(args->RelativeToPath) : nullptr,
		(svn_depth_t)args->Depth,
		args->IgnoreAncestry,
		args->NoAdded,
		args->NoDeleted,
		args->CopiesAsAdds,
		args->IgnoreContentType,
		args->NoProperties,
		args->PropertiesOnly,
		args->UseGitFormat,
		pool.AllocString(args->HeaderEncoding),
		outstream,
		errstream,
		CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
		CtxHandle,
		pool.Handle);

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
	ArgsStore store(this, args, %pool);

	AprStreamFile out(result, %pool);
	AprStreamFile err(args->ErrorStream ? args->ErrorStream : gcnew System::IO::MemoryStream(), %pool);

	svn_stream_t *outstream = svn_stream_from_aprfile2(out.CreateHandle(), TRUE, pool.Handle);
	svn_stream_t *errstream = svn_stream_from_aprfile2(err.CreateHandle(), TRUE, pool.Handle);

	svn_opt_revision_t pegRev = source->Revision->ToSvnRevision();
	svn_opt_revision_t fromRev = range->StartRevision->ToSvnRevision();
	svn_opt_revision_t toRev = range->EndRevision->ToSvnRevision();

	ICollection<String^>^ diffArgs = args->DiffArguments;

	if (!diffArgs)
		diffArgs = safe_cast<IList<String^>^>(gcnew array<String^>(0));

	svn_error_t *r = svn_client_diff_peg6(
		AllocArray(diffArgs, %pool),
		source->AllocAsString(%pool),
		&pegRev,
		&fromRev,
		&toRev,
		args->RelativeToPath ? pool.AllocDirent(args->RelativeToPath) : nullptr,
		(svn_depth_t)args->Depth,
		args->IgnoreAncestry,
		args->NoAdded,
		args->NoDeleted,
		args->CopiesAsAdds,
		args->IgnoreContentType,
		args->NoProperties,
		args->PropertiesOnly,
		args->UseGitFormat,
		pool.AllocString(args->HeaderEncoding),
		outstream,
		errstream,
		CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r, source);
}
