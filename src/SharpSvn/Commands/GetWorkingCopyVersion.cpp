// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/GetWorkingCopyVersion.h"
#include "Args/SvnWorkingCopyVersion.h"

#include "UnmanagedStructs.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;


bool SvnWorkingCopyClient::GetVersion(String^ targetPath, [Out] SvnWorkingCopyVersion^% version)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");

	return GetVersion(targetPath, gcnew SvnGetWorkingCopyVersionArgs(), version);
}

bool SvnWorkingCopyClient::GetVersion(String^ targetPath, SvnGetWorkingCopyVersionArgs^ args, [Out] SvnWorkingCopyVersion^% version)
{
	if (String::IsNullOrEmpty(targetPath))
		throw gcnew ArgumentNullException("targetPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_wc_revision_status_t *result = nullptr;

	svn_error_t *r = svn_wc_revision_status(
		&result,
		pool.AllocPath(targetPath),
		args->Trail ? pool.AllocString(args->Trail) : nullptr,
		args->UseCommittedRevisions,
		CtxHandle->cancel_func,
		CtxHandle->cancel_baton,
		pool.Handle);

	if (result)
		version = gcnew SvnWorkingCopyVersion(result->min_rev, result->max_rev, result->switched != 0, result->modified != 0, result->sparse_checkout != 0);
	else
		version = nullptr;

	return args->HandleResult(this, r);
}
