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
#include "Args/Commit.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace Microsoft::Win32;
using System::IO::File;
using System::IO::StreamWriter;

[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Commit(System.Collections.Generic.ICollection`1<System.String>,SharpSvn.SvnCommitArgs,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Commit(System.Collections.Generic.ICollection`1<System.String>,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Commit(System.String,SharpSvn.SvnCommitArgs,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="2#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.Commit(System.String,SharpSvn.SvnCommitResult&):System.Boolean", MessageId="1#")];

bool SvnClient::Commit(String^ path)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	SvnCommitResult^ result = nullptr;

	return Commit(NewSingleItemCollection(path), gcnew SvnCommitArgs(), result);
}

bool SvnClient::Commit(String^ path, [Out] SvnCommitResult^% result)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	return Commit(NewSingleItemCollection(path), gcnew SvnCommitArgs(), result);
}

bool SvnClient::Commit(ICollection<String^>^ paths)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");

	SvnCommitResult^ result = nullptr;

	return Commit(paths, gcnew SvnCommitArgs(), result);
}

bool SvnClient::Commit(ICollection<String^>^ paths, [Out] SvnCommitResult^% result)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");

	return Commit(paths, gcnew SvnCommitArgs(), result);
}

bool SvnClient::Commit(String^ path, SvnCommitArgs^ args)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitResult^ result = nullptr;

	return Commit(NewSingleItemCollection(path), args, result);
}

bool SvnClient::Commit(String^ path, SvnCommitArgs^ args, [Out] SvnCommitResult^% result)
{
	if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	return Commit(NewSingleItemCollection(path), args, result);
}

bool SvnClient::Commit(ICollection<String^>^ paths, SvnCommitArgs^ args)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitResult^ result = nullptr;

	return Commit(paths, args, result);
}

bool SvnClient::Commit(ICollection<String^>^ paths, SvnCommitArgs^ args, [Out] SvnCommitResult^% result)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (paths->Count == 0)
		throw gcnew ArgumentException(SharpSvnStrings::CollectionMustContainAtLeastOneItem, "paths");

	for each (String^ path in paths)
	{
		if (String::IsNullOrEmpty(path))
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "paths");
		else if (!IsNotUri(path))
			throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "paths");
	}

	EnsureState(SvnContextState::AuthorizationInitialized, args->RunTortoiseHooks ? SvnExtendedState::TortoiseSvnHooksLoaded : SvnExtendedState::None);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);
	CommitResultReceiver crr(this);

	AprArray<String^, AprCStrDirentMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCStrDirentMarshaller^>(paths, %pool);

	String^ commonPath = nullptr;
	String^ pathsFile = nullptr;
	String^ msgFile = nullptr;
	SvnClientHook ^preCommitHook = nullptr;
	SvnClientHook ^postCommitHook = nullptr;

	if (args->RunTortoiseHooks)
	{
		const char *pCommonPath;
		SVN_HANDLE(svn_dirent_condense_targets(&pCommonPath, NULL, aprPaths->Handle, FALSE, pool.Handle, pool.Handle));
		if (pCommonPath && pCommonPath[0] != '\0')
		{
			commonPath = Utf8_PathPtrToString(pCommonPath, %pool);
		}

		if (!String::IsNullOrEmpty(commonPath))
		{
			FindHook(commonPath, SvnClientHookType::PreCommit, preCommitHook);
			FindHook(commonPath, SvnClientHookType::PostCommit, postCommitHook);
		}

		if (preCommitHook || postCommitHook)
		{
			AprPool subpool(%pool);
			const char *path;
			svn_stream_t *f;
			const apr_array_header_t *h = aprPaths->Handle;

			/* Delete the tempfile on disposing the SvnClient */
			SVN_HANDLE(svn_stream_open_unique(&f, &path, nullptr, svn_io_file_del_on_pool_cleanup,
											  _pool.Handle, subpool.Handle));

			for (int i = 0; i < h->nelts; i++)
			{
				SVN_HANDLE(svn_stream_printf(f, subpool.Handle, "%s\n",
											 svn_dirent_local_style(APR_ARRAY_IDX(h, i, const char *), subpool.Handle)));
			}
			SVN_HANDLE(svn_stream_close(f));
			pathsFile = Utf8_PathPtrToString(path, %subpool);

			/* Delete the tempfile on disposing the SvnClient */
			SVN_HANDLE(svn_stream_open_unique(&f, &path, nullptr, svn_io_file_del_on_pool_cleanup,
											  _pool.Handle, subpool.Handle));

			SVN_HANDLE(svn_stream_printf(f, subpool.Handle, "%s",
										 subpool.AllocString(args->LogMessage)));
			SVN_HANDLE(svn_stream_close(f));

			msgFile = Utf8_PathPtrToString(path, %subpool);
		}
	}

	if (preCommitHook != nullptr)
	{
		if (!preCommitHook->Run(this, args,
								pathsFile,
								((int)args->Depth).ToString(CultureInfo::InvariantCulture),
								msgFile,
								commonPath))
			return false;
	}

	svn_error_t *r = svn_client_commit6(
		aprPaths->Handle,
		(svn_depth_t)args->Depth,
		args->KeepLocks,
		args->KeepChangeLists,
		TRUE,
		args->IncludeFileExternals,
		args->IncludeDirectoryExternals,
		CreateChangeListsList(args->ChangeLists, %pool), // Intersect ChangeLists
		CreateRevPropList(args->LogProperties, %pool),
		crr.CommitCallback, crr.CommitBaton,
		CtxHandle,
		pool.Handle);

	result = crr.CommitResult;

	if (postCommitHook != nullptr)
	{
		AprPool subpool(%pool);
		const char *path;
		svn_stream_t *f;
		char *tmpBuf = (char*)subpool.Alloc(1024);

		/* Delete the tempfile on disposing the SvnClient */
		SVN_HANDLE(svn_stream_open_unique(&f, &path, nullptr, svn_io_file_del_on_pool_cleanup,
										  _pool.Handle, subpool.Handle));

		svn_error_t *rr = r;

		while(rr)
		{
			SVN_HANDLE(svn_stream_printf(f, subpool.Handle, "%s\n",
										 svn_err_best_message(rr, tmpBuf, 1024)));

			rr = rr->child;
		}

		SVN_HANDLE(svn_stream_close(f));
		String^ errFile = Utf8_PathPtrToString(path, %subpool);

		if (!postCommitHook->Run(this, args,
								 pathsFile,
								 ((int)args->Depth).ToString(CultureInfo::InvariantCulture),
								 msgFile,
								 (result ? result->Revision : -1).ToString(CultureInfo::InvariantCulture),
								 errFile,
								 commonPath))
			return false;
	}

	return args->HandleResult(this, r, paths);
}
