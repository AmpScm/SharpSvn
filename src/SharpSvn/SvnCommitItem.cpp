// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#include "SvnAll.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;

SvnCommitResult^ SvnCommitResult::Create(SvnClient^ client, SvnClientArgs^ args, const svn_commit_info_t *commitInfo, AprPool^ pool)
{
	if (!client)
		throw gcnew ArgumentNullException("client");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!pool)
		throw gcnew ArgumentNullException("pool");

	if(!commitInfo || (commitInfo->revision <= 0L))
		return nullptr;

	return gcnew SvnCommitResult(commitInfo, pool);
}

SvnCommitResult::SvnCommitResult(const svn_commit_info_t *commitInfo, AprPool^ pool)
{
	if (!commitInfo)
		throw gcnew ArgumentNullException("commitInfo");
	else if(!pool)
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
	_postCommitError = commitInfo->post_commit_err ? SvnBase::Utf8_PtrToString(commitInfo->post_commit_err) : nullptr;
}

SvnCommitItem::SvnCommitItem(const svn_client_commit_item3_t *commitItemInfo)
{
	if (!commitItemInfo)
		throw gcnew ArgumentNullException("commitItemInfo");

	_info = commitItemInfo;
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
	}
}

