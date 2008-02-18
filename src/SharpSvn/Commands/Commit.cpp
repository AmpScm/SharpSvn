// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/Commit.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Commit(System.Collections.Generic.ICollection`1<System.String>,SharpSvn.SvnCommitArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Commit(System.Collections.Generic.ICollection`1<System.String>,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Commit(System.String,SharpSvn.SvnCommitArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Commit(System.String,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="1#")];

bool SvnClient::Commit(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	SvnCommitInfo^ commitInfo = nullptr;

	return Commit(NewSingleItemCollection(path), gcnew SvnCommitArgs(), commitInfo);
}

bool SvnClient::Commit(String^ path, [Out] SvnCommitInfo^% commitInfo)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	return Commit(NewSingleItemCollection(path), gcnew SvnCommitArgs(), commitInfo);
}

bool SvnClient::Commit(ICollection<String^>^ paths)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");

	SvnCommitInfo^ commitInfo = nullptr;

	return Commit(paths, gcnew SvnCommitArgs(), commitInfo);
}

bool SvnClient::Commit(ICollection<String^>^ paths, [Out] SvnCommitInfo^% commitInfo)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");

	return Commit(paths, gcnew SvnCommitArgs(), commitInfo);
}

bool SvnClient::Commit(String^ path, SvnCommitArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitInfo^ commitInfo = nullptr;

	return Commit(NewSingleItemCollection(path), args, commitInfo);
}

bool SvnClient::Commit(String^ path, SvnCommitArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return Commit(NewSingleItemCollection(path), args, commitInfo);
}

bool SvnClient::Commit(ICollection<String^>^ paths, SvnCommitArgs^ args)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitInfo^ commitInfo = nullptr;

	return Commit(paths, args, commitInfo);
}

bool SvnClient::Commit(ICollection<String^>^ paths, SvnCommitArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	for each (String^ path in paths)
	{
		if (String::IsNullOrEmpty(path))
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "paths");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprArray<String^, AprCStrPathMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCStrPathMarshaller^>(paths, %pool);

	svn_commit_info_t *commitInfoPtr = nullptr;

	svn_error_t *r = svn_client_commit4(
		&commitInfoPtr,
		aprPaths->Handle,
		(svn_depth_t)args->Depth,
		args->KeepLocks,
		args->KeepChangeList,
		CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
		CtxHandle,
		pool.Handle);

	if (commitInfoPtr)
		commitInfo = SvnCommitInfo::Create(this, args, commitInfoPtr, %pool);
	else
		commitInfo = nullptr;

	return args->HandleResult(this, r);
}
