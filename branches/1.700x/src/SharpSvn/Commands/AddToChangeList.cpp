// $Id$
//
// Copyright 2008-2009 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

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
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);


	svn_error_t *r = svn_client_add_to_changelist(
		AllocDirentArray(targets, %pool),
		pool.AllocString(changeList),
		(svn_depth_t)args->Depth,
		CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r, targets);
}
