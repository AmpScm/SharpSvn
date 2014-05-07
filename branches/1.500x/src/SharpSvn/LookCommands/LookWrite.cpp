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
#include "LookArgs/SvnLookWriteArgs.h"
#include "SvnStreamWrapper.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn;
using namespace SharpSvn::Implementation;


bool SvnLookClient::Write(SvnLookOrigin^ lookOrigin, String^ path, Stream^ toStream)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!toStream)
		throw gcnew ArgumentNullException("toStream");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	return Write(lookOrigin, path, toStream, gcnew SvnLookWriteArgs());
}


bool SvnLookClient::Write(SvnLookOrigin^ lookOrigin, String^ path, Stream^ toStream, SvnLookWriteArgs^ args)
{
	if (!lookOrigin)
		throw gcnew ArgumentNullException("lookOrigin");
	else if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!toStream)
		throw gcnew ArgumentNullException("toStream");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);
	
	svn_revnum_t base_rev = 0;
	svn_fs_root_t* root = nullptr;
	svn_error_t* r = open_origin(lookOrigin, &root, nullptr, nullptr, %pool);

	if (r)
		return args->HandleResult(this, r);

	svn_stream_t* fstream;

	if (r = svn_fs_file_contents(&fstream, root, pool.AllocCanonical(path), pool.Handle))
		return args->HandleResult(this, r);

	SvnStreamWrapper wrapper(toStream, false, true, %pool);
	char *buf = (char*)pool.Alloc(SVN__STREAM_CHUNK_SIZE);
	apr_size_t len;

	do
	{
		len = SVN__STREAM_CHUNK_SIZE;
		
		if (r = svn_stream_read(fstream, buf, &len))
			return args->HandleResult(this, r);
		if (r = svn_stream_write(wrapper.Handle, buf, &len))
			return args->HandleResult(this, r);
	}
	while (len == SVN__STREAM_CHUNK_SIZE);

	return SVN_NO_ERROR;
}
