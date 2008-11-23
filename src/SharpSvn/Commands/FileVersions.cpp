// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/FileVersions.h"
#include <svn_ra.h>

#include "UnmanagedStructs.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

#pragma warning(disable: 4947) // Internal obsoletes

bool SvnClient::FileVersions(SvnTarget^ target, EventHandler<SvnFileVersionEventArgs^>^ versionHandler)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!versionHandler)
		throw gcnew ArgumentNullException("versionHandler");

	return FileVersions(target, gcnew SvnFileVersionsArgs(), versionHandler);
}

struct file_version_delta_baton_t
{
	void* clientBaton;

	svn_txdelta_window_handler_t wrapped_handler;
	void *wrapped_baton;

	apr_file_t *source_file;  /* the delta source */
	apr_file_t *file;  /* the result of the delta */
	const char *filename;
};

static svn_error_t *file_version_window_handler(
	svn_txdelta_window_t *window, 
	void *baton)
{
	file_version_delta_baton_t *dbaton = (file_version_delta_baton_t*)baton;

	svn_error_t * r = dbaton->wrapped_handler(window, dbaton->wrapped_baton);

	if(r)
		return r;

	if (window)
		return nullptr; // We are only interested after 

	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)dbaton->clientBaton);	

	SvnFileVersionsArgs^ args = dynamic_cast<SvnFileVersionsArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs
	if (!args)
		return nullptr;

	if (dbaton->source_file)
		svn_io_file_close(dbaton->source_file, args->_curPool->Handle);

	svn_io_file_close(dbaton->file, args->_curPool->Handle);

	AprPool^ next = args->_prevPool;


	// Clean up for the next round
	args->_lastFile = dbaton->filename;
	next->Clear();

	args->_prevPool = args->_curPool;
	args->_curPool = next;

	return nullptr;
}

#pragma warning(disable: 4127) // conditional expression is constant on SVN_ERR

/* Apply property changes on the current property state and copy everything to the new pool
* to make sure the properties are still valid in the next round
*/
static svn_error_t*
apply_property_changes(SvnFileVersionsArgs^ args, apr_array_header_t *props, AprPool^ allocPool, AprPool^ tmpPool)
{
	apr_hash_t* oldProps = args->_properties;
	apr_hash_t* newProps = apr_hash_make(allocPool->Handle);

	bool svnOnly = args->RetrieveProperties;

	if (props && props->nelts)
	{
		// Set new properties and remove removed properties
		for (int i = 0; i < props->nelts; i++)
		{
			svn_prop_t *prop = &APR_ARRAY_IDX(props, i, svn_prop_t);

			if (svnOnly && !svn_prop_is_svn_prop(prop->name))
				continue;

			if (prop->value)
				apr_hash_set(newProps, apr_pstrdup(allocPool->Handle, prop->name), APR_HASH_KEY_STRING, 
				svn_string_dup(prop->value, allocPool->Handle));
			else if(oldProps)
				apr_hash_set(oldProps, prop->name, APR_HASH_KEY_STRING, nullptr); // Remove from old hash
		}
	}

	if (oldProps)
	{
		// Copy old properties
		for (apr_hash_index_t* hi = apr_hash_first(tmpPool->Handle, oldProps); hi; hi = apr_hash_next(hi))
		{
			svn_string_t *val;
			const char* key;
			apr_hash_this(hi, (const void**)&key, NULL, (void**)&val);

			if (!apr_hash_get(newProps, key, APR_HASH_KEY_STRING))
			{
				apr_hash_set(newProps, 
					apr_pstrdup(allocPool->Handle, key), APR_HASH_KEY_STRING, 
					svn_string_dup(val, allocPool->Handle));
			}
		}
	}

	args->_properties = newProps;

	return nullptr;
}


static svn_error_t *file_version_handler(
	void *baton,
	const char *path,
	svn_revnum_t rev,
	apr_hash_t *rev_props,
	svn_boolean_t result_of_merge,
	svn_txdelta_window_handler_t *content_delta_handler,
	void **content_delta_baton,
	apr_array_header_t *prop_diffs,
	apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)baton);	

	AprPool thePool(pool, false);

	SvnFileVersionsArgs^ args = dynamic_cast<SvnFileVersionsArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs
	if (!args)
		return nullptr;

	// <CancelChecking> // We replace the client layer here; we must check for cancel
	SvnCancelEventArgs^ cA = gcnew SvnCancelEventArgs();

	client->HandleClientCancel(cA);

	if(cA->Cancel)
		return svn_error_create (SVN_ERR_CANCELLED, nullptr, "Operation canceled");
	// </CancelChecking>

	// <Update Property List>
	SVN_ERR(apply_property_changes(args, prop_diffs, args->_curPool, %thePool));
	// </Update Property List>

	SvnFileVersionEventArgs^ e = gcnew SvnFileVersionEventArgs(
		nullptr, 
		path, 
		rev,
		rev_props, 
		args->RetrieveProperties ? args->_properties : nullptr,
		result_of_merge!=0, %thePool);

	try
	{
		// <CancelChecking> 
		client->HandleClientCancel(cA);

		if(cA->Cancel)
			return svn_error_create (SVN_ERR_CANCELLED, nullptr, "Operation canceled");
		// </CancelChecking>        

		if(content_delta_handler && content_delta_baton)
		{
			AprPool ^_pool = args->_curPool;

			file_version_delta_baton_t* delta_baton = (file_version_delta_baton_t*)_pool->AllocCleared(sizeof(file_version_delta_baton_t));

			delta_baton->clientBaton = baton;

			if (args->_lastFile)
				SVN_ERR(svn_io_file_open(&delta_baton->source_file, args->_lastFile,
				APR_READ, APR_OS_DEFAULT, _pool->Handle));
			else
				/* Means empty stream below. */
				delta_baton->source_file = NULL;

			svn_stream_t* last_stream = svn_stream_from_aprfile(delta_baton->source_file, _pool->Handle);
			AprPool^ filePool;

			if (args->_curFilePool && !result_of_merge)
				filePool = args->_curFilePool;
			else
				filePool = args->_curPool;

			SVN_ERR(svn_io_open_unique_file2(&delta_baton->file,
				&delta_baton->filename,
				args->_tempPath,
				".tmp", svn_io_file_del_on_pool_cleanup,
				filePool->Handle));
			svn_stream_t* cur_stream = svn_stream_from_aprfile(delta_baton->file, _pool->Handle);

			/* Get window handler for applying delta. */
			svn_txdelta_apply(last_stream, cur_stream, NULL, NULL,
				_pool->Handle,
				&delta_baton->wrapped_handler,
				&delta_baton->wrapped_baton);

			/* Wrap the window handler with our own. */
			*content_delta_handler = file_version_window_handler;
			*content_delta_baton = delta_baton;

			args->OnFileVersion(e);

			if (e->Cancel)
				return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Version receiver canceled operation");
		}

		return nullptr;
	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("List receiver", e);
	}
	finally
	{
		e->Detach(false);
	}
}

/*****
* BH: I use the internal libsvn_client api here to get to these things
* I had two options:
*   * Either: duplicate the code here and keep that code stable
*   * Or: Use the internal code and make that a dependency
*
* For the current testcode I chose the second option; going forward we
* should probably duplicate the code.
*
* (We already link staticly so it can't break on a user machine)
*/
#include "svn-internal/libsvn_client/client.h"

#ifdef GetTempPath
#undef GetTempPath
#endif

bool SvnClient::FileVersions(SvnTarget^ target, SvnFileVersionsArgs^ args, EventHandler<SvnFileVersionEventArgs^>^ versionHandler)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (args->Start == SvnRevision::Working || args->End == SvnRevision::Working)
		throw gcnew ArgumentException(SharpSvnStrings::RevisionTypeCantBeWorking, "args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if (versionHandler)
		args->FileVersion += versionHandler;
	try
	{
		svn_ra_session_t* ra_session = nullptr;
		const char* pTarget = pool.AllocString(target->SvnTargetName);
		const char* pUrl = nullptr;
		svn_revnum_t end_rev = 0;
		svn_revnum_t start_rev = 0;

		svn_error_t* r;

		r = svn_client__ra_session_from_path(
			&ra_session, 
			&end_rev,
			&pUrl,
			pTarget,			
			nullptr, 
			target->Revision->AllocSvnRevision(%pool),
			args->End->AllocSvnRevision(%pool), 
			CtxHandle, 
			pool.Handle);

		if (r)
			return args->HandleResult(this, r);

		r = svn_client__get_revision_number(
			&start_rev,
			nullptr,
			ra_session,
			args->Start->AllocSvnRevision(%pool),
			pTarget, 
			pool.Handle);

		if (r)
			return args->HandleResult(this, r);

		args->_curPool = gcnew AprPool(%pool);
		args->_prevPool = gcnew AprPool(%pool);
		args->_curFilePool = nullptr;
		args->_prevFilePool = nullptr;
		args->_tempPath = pool.AllocPath(System::IO::Path::GetTempPath());
		args->_lastFile = nullptr;
		args->_properties = nullptr;

		if (args->RetrieveMergedRevisions)
		{
			args->_curFilePool = gcnew AprPool(%pool);
			args->_prevFilePool = gcnew AprPool(%pool);
		}		

		r = svn_ra_get_file_revs2(
			ra_session, 
			"", // We opened the repository at the right spot
			start_rev - (start_rev > 0 ? 1 : 0),
			end_rev, 
			args->RetrieveMergedRevisions,
			file_version_handler,
			(void*)_clientBatton->Handle, 
			pool.Handle);

		return args->HandleResult(this, r);
	}
	finally
	{
		args->_curPool = nullptr;
		args->_prevPool = nullptr;
		args->_curFilePool = nullptr;
		args->_prevFilePool = nullptr;

		args->_lastFile = nullptr;
		args->_properties = nullptr;

		if (versionHandler)
			args->FileVersion -= versionHandler;
	}


	return false;
}

bool SvnClient::GetFileVersions(SvnTarget^ target, [Out] Collection<SvnFileVersionEventArgs^>^% list)
{
	if (!target)
		throw gcnew ArgumentNullException("target");

	InfoItemCollection<SvnFileVersionEventArgs^>^ results = gcnew InfoItemCollection<SvnFileVersionEventArgs^>();

	try
	{
		return FileVersions(target, gcnew SvnFileVersionsArgs(), results->Handler);
	}
	finally
	{
		list = results;
	}
}

bool SvnClient::GetFileVersions(SvnTarget^ target, SvnFileVersionsArgs^ args, [Out] Collection<SvnFileVersionEventArgs^>^% list)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnFileVersionEventArgs^>^ results = gcnew InfoItemCollection<SvnFileVersionEventArgs^>();

	try
	{
		return FileVersions(target, args, results->Handler);
	}
	finally
	{
		list = results;
	}
}
