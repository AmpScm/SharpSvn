// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::RemoveFromChangelist(String^ path)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	return RemoveFromChangelist(NewSingleItemCollection(path), gcnew SvnRemoveFromChangelistArgs());
}

bool SvnClient::RemoveFromChangelist(String^ path, SvnRemoveFromChangelistArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return RemoveFromChangelist(NewSingleItemCollection(path), args);
}

bool SvnClient::RemoveFromChangelist(ICollection<String^>^ paths)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");

	return RemoveFromChangelist(paths, gcnew SvnRemoveFromChangelistArgs());
}

bool SvnClient::RemoveFromChangelist(ICollection<String^>^ paths, SvnRemoveFromChangelistArgs^ args)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_error_t *r = svn_client_remove_from_changelists(
		AllocPathArray(paths, %pool),
		(svn_depth_t)args->Depth,
		CreateChangelistsList(args->Changelists, %pool), // Intersect Changelists
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}