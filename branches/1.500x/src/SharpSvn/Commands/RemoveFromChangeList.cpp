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
#include "Args/RemoveFromChangeList.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::RemoveFromChangeList(String^ target)
{
	if (String::IsNullOrEmpty(target))
		throw gcnew ArgumentNullException("target");
	else if (!IsNotUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");

	return RemoveFromChangeList(NewSingleItemCollection(target), gcnew SvnRemoveFromChangeListArgs());
}

bool SvnClient::RemoveFromChangeList(String^ target, SvnRemoveFromChangeListArgs^ args)
{
	if (String::IsNullOrEmpty(target))
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(target))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "target");

	return RemoveFromChangeList(NewSingleItemCollection(target), args);
}

bool SvnClient::RemoveFromChangeList(ICollection<String^>^ targets)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");

	return RemoveFromChangeList(targets, gcnew SvnRemoveFromChangeListArgs());
}

bool SvnClient::RemoveFromChangeList(ICollection<String^>^ targets, SvnRemoveFromChangeListArgs^ args)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_error_t *r = svn_client_remove_from_changelists(
		AllocPathArray(targets, %pool),
		(svn_depth_t)args->Depth,
		CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}