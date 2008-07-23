// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

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