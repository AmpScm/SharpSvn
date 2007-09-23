#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::GetAvailableMergeInfo(SvnTarget ^target, Uri^ sourceUri, [Out]SvnAvailableMergeInfo^% mergeInfo)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!sourceUri)
		throw gcnew ArgumentNullException("sourceUri");
	
	GetAvailableMergeInfo(target, sourceUri, gcnew SvnGetSuggestedMergeSourcesArgs(), mergeInfo);
}

bool SvnClient::GetAvailableMergeInfo(SvnTarget ^target, Uri^ sourceUri, SvnGetSuggestedMergeSourcesArgs^ args, [Out]SvnAvailableMergeInfo^% mergeInfo)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!sourceUri)
		throw gcnew ArgumentNullException("sourceUri");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	apr_array_header_t* svnMergeInfo = nullptr;

	svn_error_t* err = svn_client_mergeinfo_get_available(
		&svnMergeInfo,
		pool.AllocString(target->TargetName),
		target->Revision->AllocSvnRevision(%pool),
		pool.AllocCanonical(sourceUri->ToString()),
		CtxHandle,
		pool.Handle);

	if(!err && svnMergeInfo)
	{
		mergeInfo = gcnew SvnAvailableMergeInfo(target, svnMergeInfo, %pool);
	}
	else
		mergeInfo = nullptr;

	return args->HandleResult(err) && mergeInfo;
}