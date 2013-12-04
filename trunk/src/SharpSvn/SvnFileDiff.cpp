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

#include "SvnFileDiff.h"
#include "SvnStreamWrapper.h"

using namespace SharpSvn::Diff;
using namespace System::IO;

SvnFileDiff::SvnFileDiff(svn_diff_t *diff, const char* originalPath, const char* modifiedPath,
						 const char* latestPath, const char* ancestorPath, AprPool^ pool)
	: SvnDiff(diff, pool)
{
	if (!originalPath)
		throw gcnew ArgumentNullException("originalPath");
	else if (!modifiedPath)
		throw gcnew ArgumentNullException("modifiedPath");

	_originalPath = originalPath;
	_modifiedPath = modifiedPath;
	_latestPath = latestPath;
	_ancestorPath = ancestorPath;
}

bool SvnFileDiff::TryCreate(String^ originalPath, String^ modifiedPath, SvnFileDiffArgs^ options, [Out] SvnFileDiff^% diff)
{
	return TryCreate(originalPath, modifiedPath, nullptr, nullptr, options, diff);
}

bool SvnFileDiff::TryCreate(String^ originalPath, String^ modifiedPath, String^ latestPath, SvnFileDiffArgs^ options, [Out] SvnFileDiff^% diff)
{
	return TryCreate(originalPath, modifiedPath, latestPath, nullptr, options, diff);
}

bool SvnFileDiff::TryCreate(String^ originalPath, String^ modifiedPath, String^ latestPath, String^ ancestorPath, SvnFileDiffArgs^ args, [Out] SvnFileDiff^% diff)
{
	if (String::IsNullOrEmpty(originalPath))
		throw gcnew ArgumentNullException("originalPath");
	else if (String::IsNullOrEmpty(modifiedPath))
		throw gcnew ArgumentNullException("modifiedPath");
	else if ((latestPath || ancestorPath) && String::IsNullOrEmpty(latestPath))
		throw gcnew ArgumentNullException("latestPath");
	else if (ancestorPath && String::IsNullOrEmpty(ancestorPath))
		throw gcnew ArgumentNullException("ancestorPath");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	if (originalPath && !File::Exists(originalPath))
		throw gcnew FileNotFoundException(String::Format(SharpSvnStrings::FileXNotFound, originalPath), originalPath);
	else if (modifiedPath && !File::Exists(modifiedPath))
		throw gcnew FileNotFoundException(String::Format(SharpSvnStrings::FileXNotFound, modifiedPath), modifiedPath);
	else if (latestPath && !File::Exists(latestPath))
		throw gcnew FileNotFoundException(String::Format(SharpSvnStrings::FileXNotFound, latestPath), latestPath);
	else if (ancestorPath && !File::Exists(ancestorPath))
		throw gcnew FileNotFoundException(String::Format(SharpSvnStrings::FileXNotFound, ancestorPath), ancestorPath);

	diff = nullptr;
	AprPool^ pool = gcnew AprPool();
	try
	{
		const char* origPath = pool->AllocDirent(originalPath);
		const char* modPath = pool->AllocDirent(modifiedPath);
		const char* latPath = latestPath ? pool->AllocDirent(latestPath) : nullptr;
		const char* ancPath = ancestorPath ? pool->AllocDirent(ancestorPath) : nullptr;

		svn_diff_file_options_t *options = svn_diff_file_options_create(pool->Handle);

		options->ignore_eol_style = args->IgnoreLineEndings;
		options->ignore_space = (svn_diff_file_ignore_space_t)args->IgnoreSpacing;
		options->show_c_function = args->ShowCFunction;

		svn_diff_t* dp = nullptr;
		svn_error_t* err;
		if (ancestorPath)
			err = svn_diff_file_diff4_2(&dp, origPath, modPath, latPath, ancPath, options, pool->Handle);
		else if (latestPath)
			err = svn_diff_file_diff3_2(&dp, origPath, modPath, latPath, options, pool->Handle);
		else
			err = svn_diff_file_diff_2(&dp, origPath, modPath, options, pool->Handle);

		if (err)
			return args->HandleResult(nullptr, err);
		else if (!dp)
			return false;

		diff = gcnew SvnFileDiff(dp, origPath, modPath, latPath, ancPath, pool);

		return true;
	}
	finally
	{
		if (!diff)
			delete pool;
	}
}

bool SvnFileDiff::WriteMerged(Stream^ toStream, SvnDiffWriteMergedArgs^ args)
{
	if (!toStream)
		throw gcnew ArgumentNullException("toStream");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	AprPool pool(Pool);
	SvnStreamWrapper wrapper(toStream, false, true, %pool);

	svn_error_t * err = svn_diff_file_output_merge2(
		wrapper.Handle,
		DiffHandle,
		_originalPath,
		_modifiedPath,
		_latestPath,
		_ancestorPath,
		args->ConflictModified ? pool.AllocString(args->ConflictModified) : nullptr,
		args->ConflictLatest ? pool.AllocString(args->ConflictLatest) : nullptr,
		args->ConflictSeparator ? pool.AllocString(args->ConflictSeparator) : nullptr,
		(svn_diff_conflict_display_style_t)args->Display, pool.Handle);

	return args->HandleResult(nullptr, err);
}

bool SvnFileDiff::WriteDifferences(Stream^ toStream, SvnDiffWriteDifferencesArgs^ args)
{
	if (!toStream)
		throw gcnew ArgumentNullException("toStream");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	AprPool pool(Pool);
	SvnStreamWrapper wrapper(toStream, false, true, %pool);

	svn_error_t * err = svn_diff_file_output_unified3(
		wrapper.Handle,
		DiffHandle,
		_originalPath,
		_modifiedPath,
		args->OriginalHeader ? pool.AllocString(args->OriginalHeader) : nullptr,
		args->ModifiedHeader ? pool.AllocString(args->ModifiedHeader) : nullptr,
		"UTF-8",
		args->RelativeToPath ? pool.AllocDirent(args->RelativeToPath) : nullptr,
		args->ShowCFunction,
		pool.Handle);

	return args->HandleResult(nullptr, err);
}
