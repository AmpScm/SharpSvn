// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

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
	else if(!to)
		throw gcnew ArgumentNullException("to");
	else if(!result)
		throw gcnew ArgumentNullException("result");

	return Diff(from, to, gcnew SvnDiffArgs(), result);
}

bool SvnClient::Diff(SvnTarget^ from, SvnTarget^ to, SvnDiffArgs^ args, Stream^ result)
{
	if (!from)
		throw gcnew ArgumentNullException("from");
	else if(!to)
		throw gcnew ArgumentNullException("to");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!result)
		throw gcnew ArgumentNullException("result");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprStreamFile out(result, %pool);
	AprStreamFile err(args->ErrorStream ? args->ErrorStream : gcnew System::IO::MemoryStream(), %pool);

	svn_opt_revision_t fromRev = from->GetSvnRevision(SvnRevision::Working, SvnRevision::Head);
	svn_opt_revision_t toRev = to->GetSvnRevision(SvnRevision::Working, SvnRevision::Head);

	ICollection<String^>^ diffArgs = args->DiffArguments;

	if (!diffArgs)
		diffArgs = safe_cast<IList<String^>^>(gcnew array<String^>(0));

	svn_error_t *r = svn_client_diff4(
		AllocArray(diffArgs, %pool),
		pool.AllocString(from->SvnTargetName),
		&fromRev,
		pool.AllocString(to->SvnTargetName),
		&toRev,
		args->RelativeToPath ? pool.AllocPath(args->RelativeToPath) : nullptr,
		(svn_depth_t)args->Depth,
		args->IgnoreAncestry,
		args->NoDeleted,
		args->IgnoreContentType,
		pool.AllocString(args->HeaderEncoding),
		out.CreateHandle(),
		err.CreateHandle(),
		CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}

bool SvnClient::Diff(SvnTarget^ source, SvnRevisionRange^ range, Stream^ result)
{
	if (!source)
		throw gcnew ArgumentNullException("source");
	else if(!range)
		throw gcnew ArgumentNullException("range");
	else if(!result)
		throw gcnew ArgumentNullException("result");

	return Diff(source, range, gcnew SvnDiffArgs(), result);
}

bool SvnClient::Diff(SvnTarget^ source, SvnRevisionRange^ range, SvnDiffArgs^ args, Stream^ result)
{
	if (!source)
		throw gcnew ArgumentNullException("source");
	else if(!range)
		throw gcnew ArgumentNullException("range");
	else if(!result)
		throw gcnew ArgumentNullException("result");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprStreamFile out(result, %pool);
	AprStreamFile err(gcnew System::IO::MemoryStream(), %pool);

	svn_opt_revision_t pegRev = source->Revision->ToSvnRevision();
	svn_opt_revision_t fromRev = range->StartRevision->ToSvnRevision();
	svn_opt_revision_t toRev = range->EndRevision->ToSvnRevision();

	ICollection<String^>^ diffArgs = args->DiffArguments;

	if (!diffArgs)
		diffArgs = safe_cast<IList<String^>^>(gcnew array<String^>(0));

	svn_error_t *r = svn_client_diff_peg4(
		AllocArray(diffArgs, %pool),
		pool.AllocString(source->SvnTargetName),
		&pegRev,
		&fromRev,
		&toRev,
		args->RelativeToPath ? pool.AllocPath(args->RelativeToPath) : nullptr,
		(svn_depth_t)args->Depth,
		args->IgnoreAncestry,
		args->NoDeleted,
		args->IgnoreContentType,
		pool.AllocString(args->HeaderEncoding),
		out.CreateHandle(),
		err.CreateHandle(),
		CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}
