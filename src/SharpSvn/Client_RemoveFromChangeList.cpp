// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.RemoveFromChangeList(System.Collections.Generic.ICollection`1<System.String>,SharpSvn.SvnRemoveFromChangeListArgs):System.Boolean")];

bool SvnClient::RemoveFromChangeList(String^ path)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	return RemoveFromChangeList(NewSingleItemCollection(path), gcnew SvnRemoveFromChangeListArgs());
}

bool SvnClient::RemoveFromChangeList(String^ path, SvnRemoveFromChangeListArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return RemoveFromChangeList(NewSingleItemCollection(path), args);
}

bool SvnClient::RemoveFromChangeList(ICollection<String^>^ paths)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");

	return RemoveFromChangeList(paths, gcnew SvnRemoveFromChangeListArgs());
}

bool SvnClient::RemoveFromChangeList(ICollection<String^>^ paths, SvnRemoveFromChangeListArgs^ args)
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
		(const apr_array_header_t *)nullptr, // Changelists
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}