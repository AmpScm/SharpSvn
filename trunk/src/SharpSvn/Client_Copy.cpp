#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::Copy(SvnTarget^ sourceTarget, String^ toPath)
{
	if(!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if(String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if(!IsNotUri(toPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	Copy(NewSingleItemCollection(sourceTarget), toPath, gcnew SvnCopyArgs());
}

void SvnClient::Copy(ICollection<SvnTarget^>^ sourceTargets, String^ toPath)
{
	if(!sourceTargets)
		throw gcnew ArgumentNullException("sourceTargets");
	else if(String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if(!IsNotUri(toPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toPath");

	Copy(sourceTargets, toPath, gcnew SvnCopyArgs());
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
		!args->WithoutMergeHistory,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}

void SvnClient::RemoteCopy(SvnUriTarget^ sourceTarget, Uri^ toUri)
{
	if(!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");

	SvnCommitInfo^ commitInfo = nullptr;

	RemoteCopy(NewSingleItemCollection(sourceTarget), toUri, gcnew SvnCopyArgs(), commitInfo);
}

void SvnClient::RemoteCopy(ICollection<SvnUriTarget^>^ sourceTargets, Uri^ toUri)
{
	if(!sourceTargets)
		throw gcnew ArgumentNullException("sourceTarget");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");

	SvnCommitInfo^ commitInfo = nullptr;

	RemoteCopy(sourceTargets, toUri, gcnew SvnCopyArgs(), commitInfo);
}

void SvnClient::RemoteCopy(SvnUriTarget^ sourceTarget, Uri^ toUri, [Out] SvnCommitInfo^% commitInfo)
{
	if(!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");

	RemoteCopy(NewSingleItemCollection(sourceTarget), toUri, gcnew SvnCopyArgs(), commitInfo);
}

void SvnClient::RemoteCopy(ICollection<SvnUriTarget^>^ sourceTargets, Uri^ toUri, [Out] SvnCommitInfo^% commitInfo)
{
	if(!sourceTargets)
		throw gcnew ArgumentNullException("sourceTargets");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");

	RemoteCopy(sourceTargets, toUri, gcnew SvnCopyArgs(), commitInfo);
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
		throw gcnew ArgumentNullException("sourceTarget");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if(!args)
		throw gcnew ArgumentNullException("args");

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
		!args->WithoutMergeHistory,
		CtxHandle,
		pool.Handle);

	if(pInfo)
		commitInfo = gcnew SvnCommitInfo(pInfo, %pool);
	else
		commitInfo = nullptr;

	return args->HandleResult(this, r);
}

