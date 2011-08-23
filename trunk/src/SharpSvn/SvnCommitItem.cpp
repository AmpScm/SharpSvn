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

using namespace SharpSvn::Implementation;
using namespace SharpSvn;

SvnCommittedEventArgs^ SvnCommittedEventArgs::Create(SvnClientContext^ client, const svn_commit_info_t *commitInfo, AprPool^ pool)
{
	if (!client)
		throw gcnew ArgumentNullException("client");
	else if (!pool)
		throw gcnew ArgumentNullException("pool");

	if (!commitInfo || (commitInfo->revision <= 0L))
		return nullptr;

	return gcnew SvnCommittedEventArgs(commitInfo, pool);
}

SvnCommitResult::SvnCommitResult(const svn_commit_info_t *commitInfo, AprPool^ pool)
{
	if (!commitInfo)
		throw gcnew ArgumentNullException("commitInfo");
	else if (!pool)
		throw gcnew ArgumentNullException("pool");

	_revision = commitInfo->revision;

	apr_time_t when = 0;
	svn_error_t *err = svn_time_from_cstring(&when, commitInfo->date, pool->Handle); // pool is not used at this time (might be for errors in future versions)

	if (!err)
		_date = SvnBase::DateTimeFromAprTime(when);
	else
	{
		svn_error_clear(err);
		_date = DateTime::MinValue;
	}

	_author = SvnBase::Utf8_PtrToString(commitInfo->author);

	_postCommitError = commitInfo->post_commit_err
			? SvnBase::Utf8_PtrToString(commitInfo->post_commit_err)->Replace("\n", Environment::NewLine)
				->Replace("\r\r", "\r")
			: nullptr;

	if (commitInfo->repos_root)
		_reposRoot = SvnBase::Utf8_PtrToUri(commitInfo->repos_root, SvnNodeKind::Directory);
}

SvnCommittedEventArgs::SvnCommittedEventArgs(const svn_commit_info_t *commitInfo, AprPool^ pool)
    : SvnCommitResult(commitInfo, pool)
{
}

SvnCommitItem::SvnCommitItem(const svn_client_commit_item3_t *commitItemInfo, AprPool^ pool)
{
	if (!commitItemInfo)
		throw gcnew ArgumentNullException("commitItemInfo");

	_info = commitItemInfo;
	_nodeKind = (SvnNodeKind)commitItemInfo->kind;
	_revision = commitItemInfo->revision;
	_copyFromRevision = commitItemInfo->copyfrom_rev;
	_commitType = (SvnCommitTypes)commitItemInfo->state_flags;
	_pool = pool;
}

void SvnCommitItem::Detach(bool keepProperties)
{
	try
	{
		if (keepProperties)
		{
			GC::KeepAlive(Path);
			GC::KeepAlive(Uri);
			GC::KeepAlive(CopyFromUri);
		}
	}
	finally
	{
		_info = nullptr;
		_pool = nullptr;
	}
}

