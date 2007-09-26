#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.GetAppliedMergeInfo(SharpSvn.SvnTarget,SharpSvn.SvnGetAppliedMergeInfoArgs,SharpSvn.SvnAppliedMergeInfo&):System.Boolean")];

void SvnClient::GetAppliedMergeInfo(SvnTarget ^target, [Out]SvnAppliedMergeInfo^% mergeState)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	GetAppliedMergeInfo(target, gcnew SvnGetAppliedMergeInfoArgs(), mergeState);
}

bool SvnClient::GetAppliedMergeInfo(SvnTarget ^target, SvnGetAppliedMergeInfoArgs^ args, [Out]SvnAppliedMergeInfo^% mergeState)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	apr_hash_t* svnMergeInfo = nullptr;

	svn_error_t* r = svn_client_mergeinfo_get_merged(
		&svnMergeInfo,
		pool.AllocString(target->TargetName),
		target->Revision->AllocSvnRevision(%pool),
		CtxHandle,
		pool.Handle);

	if(!r && svnMergeInfo)
	{
		mergeState = gcnew SvnAppliedMergeInfo(target, svnMergeInfo, %pool);
	}
	else
		mergeState = nullptr;

	return args->HandleResult(this, r) && mergeState;
}