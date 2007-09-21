#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::GetSuggestedMergeSources(SvnTarget ^target, [Out]SvnMergeSources^% mergeInfo)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	GetSuggestedMergeSources(target, gcnew SvnGetSuggestedMergeSourcesArgs(), mergeInfo);
}

bool SvnClient::GetSuggestedMergeSources(SvnTarget ^target, SvnGetSuggestedMergeSourcesArgs^ args, [Out]SvnMergeSources^% mergeInfo)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);
	mergeInfo = nullptr;

	apr_array_header_t* svnMergeSources = nullptr;

	svn_error_t* err = svn_client_suggest_merge_sources(
		&svnMergeSources,
		pool.AllocString(target->TargetName),
		target->Revision->AllocSvnRevision(%pool),
		CtxHandle,
		pool.Handle);

	if(!err)
	{
		mergeInfo = gcnew SvnMergeSources(target, svnMergeSources);
	}

	return args->HandleResult(err);
}

bool SvnClient::TryGetSuggestedMergeSources(SvnTarget ^target, [Out]SvnMergeSources^% mergeInfo)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	SvnGetSuggestedMergeSourcesArgs^ args = gcnew SvnGetSuggestedMergeSourcesArgs();
	args->ThrowOnError = false;

	if(!GetSuggestedMergeSources(target, args, mergeInfo) || !mergeInfo || !mergeInfo->Available)
	{
		mergeInfo = nullptr;
		return false;
	}

	return true;
}