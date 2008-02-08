// $Id: Client_GetSuggestedMergeSources.cpp 219 2008-02-05 11:32:46Z bhuijben $
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "UnmanagedStructs.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::GetWorkingCopyState(String^ targetPath, [Out] SvnWorkingCopyState^% result)
{
	if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");

	return GetWorkingCopyState(targetPath, gcnew SvnGetWorkingCopyStateArgs(), result);
}

bool SvnClient::GetWorkingCopyState(String^ targetPath, SvnGetWorkingCopyStateArgs^ args, [Out] SvnWorkingCopyState^% result)
{
	if(String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!IsNotUri(targetPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "targetPath");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);	
	AprPool pool(%_pool);

	const char* pPath = pool.AllocPath(targetPath);
	svn_wc_adm_access_t* acc = nullptr;

	svn_error_t* r = svn_wc_adm_probe_open3(&acc, nullptr, pPath, false, 1, CtxHandle->cancel_func, CtxHandle->cancel_baton, pool.Handle);

	if(r)
		return args->HandleResult(this, r);

	try
	{
		svn_boolean_t pIsBinary = 0;

		r = svn_wc_has_binary_prop(&pIsBinary, pPath, acc, pool.Handle);
		if(r)
			return args->HandleResult(this, r);

		const char* basePath = nullptr;
		r = svn_wc_get_pristine_copy_path(pool.AllocPath(targetPath), &basePath, pool.Handle);
	
		if(r)
			return args->HandleResult(this, r);

		result = gcnew SvnWorkingCopyState(!pIsBinary, basePath ? Utf8_PtrToString(basePath) : nullptr);
	}
	finally
	{
		svn_wc_adm_close(acc);
	}

	return true;
}