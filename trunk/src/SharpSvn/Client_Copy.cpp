// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteCopy(SharpSvn.SvnUriTarget,System.Uri,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteCopy(SharpSvn.SvnUriTarget,System.Uri,SharpSvn.SvnCopyArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteCopy(System.Collections.Generic.ICollection`1<SharpSvn.SvnUriTarget>,System.Uri,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteCopy(System.Collections.Generic.ICollection`1<SharpSvn.SvnUriTarget>,System.Uri,SharpSvn.SvnCopyArgs,SharpSvn.SvnCommitInfo&):System.Boolean", MessageId="3#")];

bool SvnClient::Copy(SvnTarget^ sourceTarget, String^ toPath)
{
	if(!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if(String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if(!IsNotUri(toPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	return Copy(NewSingleItemCollection(sourceTarget), toPath, gcnew SvnCopyArgs());
}

bool SvnClient::Copy(ICollection<SvnTarget^>^ sourceTargets, String^ toPath)
{
	if(!sourceTargets)
		throw gcnew ArgumentNullException("sourceTargets");
	else if(String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if(!IsNotUri(toPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	return Copy(sourceTargets, toPath, gcnew SvnCopyArgs());
}

bool SvnClient::Copy(SvnTarget^ sourceTarget, String^ toPath, SvnCopyArgs^ args)
{
	if(!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if(String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!IsNotUri(toPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	return Copy(NewSingleItemCollection(sourceTarget), toPath, args);
}


bool SvnClient::Copy(ICollection<SvnTarget^>^ sourceTargets, String^ toPath, SvnCopyArgs^ args)
{
	if(!sourceTargets)
		throw gcnew ArgumentNullException("sourceTargets");
	else if(String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!IsNotUri(toPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_commit_info_t* pInfo = nullptr;

	svn_error_t *r = svn_client_copy4(
		&pInfo,
		AllocCopyArray(sourceTargets, %pool),
		pool.AllocPath(toPath),
		args->AlwaysCopyAsChild || (sourceTargets->Count > 1),
		args->MakeParents,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}

bool SvnClient::RemoteCopy(SvnUriTarget^ sourceTarget, Uri^ toUri)
{
	if(!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if(!SvnBase::IsValidReposUri(toUri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "toUri");

	SvnCommitInfo^ commitInfo = nullptr;

	return RemoteCopy(NewSingleItemCollection(sourceTarget), toUri, gcnew SvnCopyArgs(), commitInfo);
}

bool SvnClient::RemoteCopy(ICollection<SvnUriTarget^>^ sourceTargets, Uri^ toUri)
{
	if(!sourceTargets)
		throw gcnew ArgumentNullException("sourceTargets");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");

	SvnCommitInfo^ commitInfo = nullptr;

	return RemoteCopy(sourceTargets, toUri, gcnew SvnCopyArgs(), commitInfo);
}

bool SvnClient::RemoteCopy(SvnUriTarget^ sourceTarget, Uri^ toUri, [Out] SvnCommitInfo^% commitInfo)
{
	if(!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");

	return RemoteCopy(NewSingleItemCollection(sourceTarget), toUri, gcnew SvnCopyArgs(), commitInfo);
}

bool SvnClient::RemoteCopy(ICollection<SvnUriTarget^>^ sourceTargets, Uri^ toUri, [Out] SvnCommitInfo^% commitInfo)
{
	if(!sourceTargets)
		throw gcnew ArgumentNullException("sourceTargets");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");

	return RemoteCopy(sourceTargets, toUri, gcnew SvnCopyArgs(), commitInfo);
}

bool SvnClient::RemoteCopy(SvnUriTarget^ sourceTarget, Uri^ toUri, SvnCopyArgs^ args)
{
	if(!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitInfo^ commitInfo = nullptr;
	return RemoteCopy(NewSingleItemCollection(sourceTarget), toUri, args, commitInfo);
}

bool SvnClient::RemoteCopy(ICollection<SvnUriTarget^>^ sourceTargets, Uri^ toUri, SvnCopyArgs^ args)
{
	if(!sourceTargets)
		throw gcnew ArgumentNullException("sourceTargets");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitInfo^ commitInfo = nullptr;
	return RemoteCopy(sourceTargets, toUri, args, commitInfo);
}

bool SvnClient::RemoteCopy(SvnUriTarget^ sourceTarget, Uri^ toUri, SvnCopyArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if(!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return RemoteCopy(NewSingleItemCollection(sourceTarget), toUri, args, commitInfo);
}


bool SvnClient::RemoteCopy(ICollection<SvnUriTarget^>^ sourceTargets, Uri^ toUri, SvnCopyArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if(!sourceTargets)
		throw gcnew ArgumentNullException("sourceTargets");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!SvnBase::IsValidReposUri(toUri))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAValidRepositoryUri, "toUri");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_commit_info_t* pInfo = nullptr;

	svn_error_t *r = svn_client_copy4(
		&pInfo,
		AllocCopyArray(sourceTargets, %pool),
		pool.AllocCanonical(toUri->ToString()),
		args->AlwaysCopyAsChild || (sourceTargets->Count > 1),
		args->MakeParents,
		CtxHandle,
		pool.Handle);

	if(pInfo)
		commitInfo = gcnew SvnCommitInfo(pInfo, %pool);
	else
		commitInfo = nullptr;

	return args->HandleResult(this, r);
}

