#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;


bool SvnClient::Resolved(String^ path)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	return Resolved(path, gcnew SvnResolvedArgs());
}

bool SvnClient::Resolved(String^ path, SvnConflictResult which)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	return Resolved(path, gcnew SvnResolvedArgs(which));
}

bool SvnClient::Resolved(String^ path, SvnResolvedArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_error_t *r = svn_client_resolved2(
		pool.AllocPath(path),
		(svn_depth_t)args->Depth,
		(svn_wc_conflict_result_t)args->Which,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}
