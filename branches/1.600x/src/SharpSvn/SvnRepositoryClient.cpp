// $Id$
//
// Copyright 2007-2008 The SharpSvn Project
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

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types


using namespace SharpSvn;
using namespace SharpSvn::Implementation;



[module: SuppressMessage("Microsoft.Naming", "CA1709:IdentifiersShouldBeCasedCorrectly", Scope="member", Target="SharpSvn.SvnRepositoryFileSystem.#FsFs", MessageId="Fs")];



static svn_error_t *
svnrepository_cancel_func(void *cancel_baton)
{
	SvnRepositoryClient^ client = AprBaton<SvnRepositoryClient^>::Get((IntPtr)cancel_baton);

	SvnCancelEventArgs^ ea = gcnew SvnCancelEventArgs();
	try
	{
		client->HandleClientCancel(ea);

		if (ea->Cancel)
			return svn_error_create (SVN_ERR_CANCELLED, nullptr, "Operation canceled from OnCancel");

		return nullptr;
	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("Cancel function", e);
	}
	finally
	{
		ea->Detach(false);
	}
}



SvnRepositoryClient::SvnRepositoryClient()
: _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
	_clientBaton = gcnew AprBaton<SvnRepositoryClient^>(this);

	CtxHandle->cancel_func = svnrepository_cancel_func;
	CtxHandle->cancel_baton = (void*)_clientBaton->Handle;
}

SvnRepositoryClient::~SvnRepositoryClient()
{
	delete _clientBaton;
}

String^ SvnRepositoryClient::FindRepositoryRoot(Uri^ repositoryUri)
{
	if (!repositoryUri)
		throw gcnew ArgumentNullException("repositoryUri");

	EnsureState(SvnContextState::ConfigLoaded);

	AprPool pool(%_pool);

	const char* root = svn_repos_find_root_path(
		pool.AllocCanonical(repositoryUri),
		pool.Handle);

	return root ? Utf8_PtrToString(root) : nullptr;
}

void SvnRepositoryClient::HandleClientCancel(SvnCancelEventArgs^ e)
{
	if (CurrentCommandArgs)
		CurrentCommandArgs->RaiseOnCancel(e);

	if (e->Cancel)
		return;

	OnCancel(e);
}

void SvnRepositoryClient::OnCancel(SvnCancelEventArgs^ e)
{
	Cancel(this, e);
}