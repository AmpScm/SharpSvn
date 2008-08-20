// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "LookArgs/SvnLookWriteArgs.h"
#include "SvnStreamWrapper.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn;
using namespace SharpSvn::Implementation;


bool SvnLookClient::Write(String^ repositoryPath, String^ path, Stream^ toStream)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!toStream)
		throw gcnew ArgumentNullException("toStream");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	return Write(repositoryPath, path, toStream, gcnew SvnLookWriteArgs());
}


bool SvnLookClient::Write(String^ repositoryPath, String^ path, Stream^ toStream, SvnLookWriteArgs^ args)
{
	if (String::IsNullOrEmpty(repositoryPath))
		throw gcnew ArgumentNullException("repositoryPath");
	else if (String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if (!toStream)
		throw gcnew ArgumentNullException("toStream");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!IsNotUri(repositoryPath))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "repositoryPath");
	else if (!IsNotUri(path))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "path");

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);
	
	svn_repos_t* repos = nullptr;
	svn_fs_t* fs = nullptr;
	svn_error_t* r;

	svn_revnum_t base_rev = 0;
	svn_fs_root_t* root = nullptr;

	if (r = svn_repos_open(
		&repos,
		pool.AllocCanonical(repositoryPath),
		pool.Handle))
	{
		return args->HandleResult(this, r);
	}

	fs = svn_repos_fs(repos);

	if (!String::IsNullOrEmpty(args->Transaction))
	{
		svn_fs_txn_t* txn = nullptr;

		if (r = svn_fs_open_txn(&txn, fs, pool.AllocString(args->Transaction), pool.Handle))
			return args->HandleResult(this, r);

		base_rev = svn_fs_txn_base_revision(txn);

		if (r = svn_fs_txn_root(&root, txn, pool.Handle))
			return args->HandleResult(this, r);				
	}
	else
	{
		svn_revnum_t rev;

		if (args->Revision < 0L)
		{
			if (r = svn_fs_youngest_rev(&rev, fs, pool.Handle))
				return args->HandleResult(this, r);
		}
		else
			rev = (svn_revnum_t)args->Revision;

		if (r = svn_fs_revision_root(&root, fs, rev, pool.Handle))
			return args->HandleResult(this, r);
	}

	svn_stream_t* fstream;

	if (r = svn_fs_file_contents(&fstream, root, pool.AllocCanonical(path), pool.Handle))
		return args->HandleResult(this, r);

	SvnStreamWrapper wrapper(toStream, false, true, %pool);
	char *buf = (char*)pool.Alloc(SVN__STREAM_CHUNK_SIZE);
	apr_size_t len;

	do
	{
		if (r = svn_stream_read(fstream, buf, &len))
			return args->HandleResult(this, r);
		if (r = svn_stream_write(wrapper.Handle, buf, &len))
			return args->HandleResult(this, r);
	}
	while (len == SVN__STREAM_CHUNK_SIZE);

	return SVN_NO_ERROR;
}

