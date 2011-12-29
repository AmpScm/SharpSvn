// $Id: Move.cpp 1710 2011-06-22 08:56:11Z rhuijben $
//
// Copyright 2007-2009 The SharpSvn Project
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
#include "WorkingCopyArgs/WorkingCopyMove.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteMove(System.Collections.Generic.ICollection`1<System.Uri>,System.Uri,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteMove(System.Collections.Generic.ICollection`1<System.Uri>,System.Uri,SharpSvn.SvnMoveArgs,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="3#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteMove(System.Uri,System.Uri,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.RemoteMove(System.Uri,System.Uri,SharpSvn.SvnMoveArgs,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="3#")];

bool SvnWorkingCopyClient::Move(String^ sourcePath, String^ toPath)
{
	if (String::IsNullOrEmpty(sourcePath))
		throw gcnew ArgumentNullException("sourcePath");
	else if (String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");

	return Move(sourcePath, toPath, gcnew SvnWorkingCopyMoveArgs());
}

bool SvnWorkingCopyClient::Move(String^ sourcePath, String^ toPath, SvnWorkingCopyMoveArgs^ args)
{
	if (String::IsNullOrEmpty(sourcePath))
		throw gcnew ArgumentNullException("sourcePath");
	else if (String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	svn_error_t *r = svn_wc_move(
		CtxHandle->wc_ctx,
		pool.AllocAbsoluteDirent(sourcePath),
		pool.AllocAbsoluteDirent(toPath),
		args->MetaDataOnly,
		CtxHandle->cancel_func, CtxHandle->cancel_baton,
		CtxHandle->notify_func2, CtxHandle->notify_baton2,
		pool.Handle);

	return args->HandleResult(this, r, sourcePath);
}
