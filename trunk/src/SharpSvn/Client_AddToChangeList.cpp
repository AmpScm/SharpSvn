// $Id: Client_ChangeList.cpp 203 2008-01-07 09:13:07Z bhuijben $
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;


[module: SuppressMessage("Microsoft.Design", "CA1011:ConsiderPassingBaseTypesAsParameters", Scope="member", Target="SharpSvn.SvnClient.AddToChangeList(System.Collections.Generic.ICollection`1<System.String>,System.String,SharpSvn.SvnAddToChangeListArgs):System.Boolean")];

bool SvnClient::AddToChangeList(String^ path, String^ changeList)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");

	return AddToChangeList(NewSingleItemCollection(path), changeList, gcnew SvnAddToChangeListArgs());
}

bool SvnClient::AddToChangeList(String^ path, String^ changeList, SvnAddToChangeListArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return AddToChangeList(NewSingleItemCollection(path), changeList, args);
}

bool SvnClient::AddToChangeList(ICollection<String^>^ paths, String^ changeList)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");

	return AddToChangeList(paths, changeList, gcnew SvnAddToChangeListArgs());
}

bool SvnClient::AddToChangeList(ICollection<String^>^ paths, String^ changeList, SvnAddToChangeListArgs^ args)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);


	svn_error_t *r = svn_client_add_to_changelist(
		AllocPathArray(paths, %pool),
		pool.AllocString(changeList),
		(svn_depth_t)args->Depth,
		(const apr_array_header_t *)nullptr, // Changelists
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}
