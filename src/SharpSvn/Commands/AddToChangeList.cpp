// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/AddToChangeList.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::AddToChangeList(String^ target, String^ changeList)
{
	if (String::IsNullOrEmpty(target))
		throw gcnew ArgumentNullException("target");
	else if (String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");

	return AddToChangeList(NewSingleItemCollection(target), changeList, gcnew SvnAddToChangeListArgs());
}

bool SvnClient::AddToChangeList(String^ target, String^ changeList, SvnAddToChangeListArgs^ args)
{
	if (String::IsNullOrEmpty(target))
		throw gcnew ArgumentNullException("target");
	else if (String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	return AddToChangeList(NewSingleItemCollection(target), changeList, args);
}

bool SvnClient::AddToChangeList(ICollection<String^>^ targets, String^ changeList)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");
	else if (String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");

	return AddToChangeList(targets, changeList, gcnew SvnAddToChangeListArgs());
}

bool SvnClient::AddToChangeList(ICollection<String^>^ targets, String^ changeList, SvnAddToChangeListArgs^ args)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");
	else if (String::IsNullOrEmpty(changeList))
		throw gcnew ArgumentNullException("changeList");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);


	svn_error_t *r = svn_client_add_to_changelist(
		AllocPathArray(targets, %pool),
		pool.AllocString(changeList),
		(svn_depth_t)args->Depth,
		CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}
