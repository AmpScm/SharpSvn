#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::GetMergeInfo(SvnTarget ^target, [Out]SvnMergeInfo^% mergeInfo)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	GetMergeInfo(target, gcnew SvnGetMergeInfoArgs(), mergeInfo);
}

bool SvnClient::GetMergeInfo(SvnTarget ^target, SvnGetMergeInfoArgs^ args, [Out]SvnMergeInfo^% mergeInfo)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	mergeInfo = nullptr;

	AprPool pool(_pool);
	_currentArgs = args;
	try
	{
		apr_hash_t* svnMergeInfo = nullptr;

		svn_error_t* err = svn_client_get_mergeinfo(
			&svnMergeInfo,
			pool.AllocString(target->ToString()), 
			target->Revision->AllocSvnRevision(%pool),
			CtxHandle,
			pool.Handle);

		if(!err)
		{
			mergeInfo = gcnew SvnMergeInfo(target, svnMergeInfo);
		}

		return args->HandleResult(err);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}

bool SvnClient::TryGetMergeInfo(SvnTarget ^target, [Out]SvnMergeInfo^% mergeInfo)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	SvnGetMergeInfoArgs^ args = gcnew SvnGetMergeInfoArgs();
	args->ThrowOnError = false;

	if(!GetMergeInfo(target, args, mergeInfo) || !mergeInfo || !mergeInfo->Available)
	{
		mergeInfo = nullptr;
		return false;
	}
	
	return true;
}