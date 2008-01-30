// $Id: Client_Merge.cpp 171 2007-10-14 19:38:35Z bhuijben $
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::ReintegrationMerge(String^ targetPath, SvnTarget^ source)
{
	if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!source)
		throw gcnew ArgumentNullException("source");

	return ReintegrationMerge(targetPath, source, gcnew SvnReintegrationMergeArgs());
}

bool SvnClient::ReintegrationMerge(String^ targetPath, SvnTarget^ source, SvnReintegrationMergeArgs^ args)
{
	if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!source)
		throw gcnew ArgumentNullException("source");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_opt_revision_t pegRev = source->GetSvnRevision(SvnRevision::Working, SvnRevision::Head);
	
	svn_error_t *r = svn_client_merge_reintegrate(
		pool.AllocString(source->TargetName),
		&pegRev,
		pool.AllocPath(targetPath),
		args->Force,
		args->DryRun,
		args->MergeArguments ? AllocArray(args->MergeArguments, %pool) : nullptr,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}