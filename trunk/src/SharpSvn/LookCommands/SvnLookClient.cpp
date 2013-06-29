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

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types


using namespace SharpSvn;
using namespace SharpSvn::Implementation;

SvnLookOrigin::SvnLookOrigin(String^ repositoryPath)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!SvnBase::IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");

	_path = SvnTools::GetNormalizedFullPath(repositoryPath);
	_revision = -1;
}

SvnLookOrigin::SvnLookOrigin(String^ repositoryPath, __int64 revision)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!SvnBase::IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");
	else if (revision < 0)
		throw gcnew ArgumentOutOfRangeException("revision", revision, SharpSvnStrings::RevisionMustBeGreaterThanOrEqualToZero);

	_path = SvnTools::GetNormalizedFullPath(repositoryPath);
	_revision = revision;
}

SvnLookOrigin::SvnLookOrigin(String^ repositoryPath, String^ transactionName)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (!SvnBase::IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");
	else if (String::IsNullOrEmpty(transactionName))
		throw gcnew ArgumentNullException("transactionName");

	_path = SvnTools::GetNormalizedFullPath(repositoryPath);
	_revision = -1;
	_transactionName = transactionName;
}

static svn_error_t *
svnlook_cancel_func(void *cancel_baton)
{
	SvnLookClient^ client = AprBaton<SvnLookClient^>::Get((IntPtr)cancel_baton);

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

SvnLookClient::SvnLookClient()
: _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
	_clientBaton = gcnew AprBaton<SvnLookClient^>(this);
	CtxHandle->cancel_func = svnlook_cancel_func;
	CtxHandle->cancel_baton = (void*)_clientBaton->Handle;
}

SvnLookClient::~SvnLookClient()
{
	delete _clientBaton;
}

void SvnLookClient::HandleClientCancel(SvnCancelEventArgs^ e)
{
	if (CurrentCommandArgs)
		CurrentCommandArgs->RaiseOnCancel(e);

	if (e->Cancel)
		return;

	OnCancel(e);
}

void SvnLookClient::OnCancel(SvnCancelEventArgs^ e)
{
	Cancel(this, e);
}

svn_error_t* SvnLookClient::open_origin(SvnLookOrigin^ lookOrigin, svn_fs_root_t **root, svn_fs_t **fs, svn_repos_t **repos, AprPool^ pool)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (!pool)
		throw gcnew ArgumentNullException("pool");

	svn_fs_root_t* pRoot = nullptr;
	svn_repos_t* pRepos = nullptr;
	svn_fs_t* pFs = nullptr;

	if (root)
		*root = nullptr;
	if (fs)
		*fs = nullptr;
	if (repos)
		*repos = nullptr;

	SVN_ERR(svn_repos_open2(&pRepos, pool->AllocDirent(lookOrigin->RepositoryPath), nullptr, pool->Handle));

	pFs = svn_repos_fs(pRepos);

	if (lookOrigin->HasTransaction)
	{
		svn_fs_txn_t* txn = nullptr;

		SVN_ERR(svn_fs_open_txn(&txn, pFs, pool->AllocString(lookOrigin->Transaction), pool->Handle));

		SVN_ERR(svn_fs_txn_root(&pRoot, txn, pool->Handle));
	}
	else
	{
		svn_revnum_t rev;

		if (!lookOrigin->HasRevision)
			SVN_ERR(svn_fs_youngest_rev(&rev, pFs, pool->Handle));
		else
			rev = (svn_revnum_t)lookOrigin->Revision;

		SVN_ERR(svn_fs_revision_root(&pRoot, pFs, rev, pool->Handle));
	}

	if (root)
		*root = pRoot;
	if (fs)
		*fs = pFs;
	if (repos)
		*repos = pRepos;

	return SVN_NO_ERROR;
}