// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::AddToChangelist(String^ path, String^ changelist)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(changelist))
		throw gcnew ArgumentNullException("changelist");

	return AddToChangelist(NewSingleItemCollection(path), changelist, gcnew SvnAddToChangelistArgs());
}

bool SvnClient::AddToChangelist(String^ path, String^ changelist, SvnAddToChangelistArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(changelist))
		throw gcnew ArgumentNullException("changelist");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return AddToChangelist(NewSingleItemCollection(path), changelist, args);
}

bool SvnClient::AddToChangelist(ICollection<String^>^ paths, String^ changelist)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(String::IsNullOrEmpty(changelist))
		throw gcnew ArgumentNullException("changelist");

	return AddToChangelist(paths, changelist, gcnew SvnAddToChangelistArgs());
}

bool SvnClient::AddToChangelist(ICollection<String^>^ paths, String^ changelist, SvnAddToChangelistArgs^ args)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(String::IsNullOrEmpty(changelist))
		throw gcnew ArgumentNullException("changelist");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);


	svn_error_t *r = svn_client_add_to_changelist(
		AllocPathArray(paths, %pool),
		pool.AllocString(changelist),
		(svn_depth_t)args->Depth,
		CreateChangelistsList(args->Changelists, %pool), // Intersect Changelists
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}
