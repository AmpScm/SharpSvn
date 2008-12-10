// $Id: Write.cpp 910 2008-11-28 00:14:38Z rhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "SvnStreamWrapper.h"
#include "Args/WriteRelated.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using System::Collections::Generic::IDictionary;
using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;
using System::Collections::Generic::List;

#ifdef _DEBUG

bool SvnClient::WriteRelated(ICollection<SvnUriTarget^>^ targets, ICollection<Stream^>^ to)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");
	else if (!to)
		throw gcnew ArgumentNullException("to");

	return WriteRelated(targets, to, gcnew SvnWriteRelatedArgs());
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

struct writerelated_node_baton
{
	void* client_baton;
	const writerelated_node_baton *parent;
	const char* path;
};

static svn_error_t * __cdecl
writerelated_set_target_revision(void *edit_baton, svn_revnum_t target_revision, apr_pool_t *pool)
{
	UNUSED_ALWAYS(edit_baton);
	UNUSED_ALWAYS(target_revision);
	UNUSED_ALWAYS(pool);
	return nullptr;
}

static svn_error_t * __cdecl
writerelated_open_root(void *edit_baton, svn_revnum_t base_revision, apr_pool_t *dir_pool, void **root_baton)
{
	UNUSED_ALWAYS(base_revision);
	writerelated_node_baton *child = (writerelated_node_baton*)apr_pcalloc(dir_pool, sizeof(writerelated_node_baton));
	child->client_baton = edit_baton;

	*root_baton = child;
	return nullptr;
}

static svn_error_t * __cdecl
writerelated_delete_entry(const char *path, svn_revnum_t revision, void *parent_baton, apr_pool_t *pool)
{
	UNUSED_ALWAYS(path);
	UNUSED_ALWAYS(pool);
	UNUSED_ALWAYS(revision);
	const writerelated_node_baton *dir = (writerelated_node_baton*)parent_baton;

	return nullptr;
}

static svn_error_t * __cdecl
writerelated_add_directory(const char *path, void *parent_baton, const char *copyfrom_path, svn_revnum_t copyfrom_revision, apr_pool_t *dir_pool, void **child_baton)
{
	UNUSED_ALWAYS(copyfrom_path);
	UNUSED_ALWAYS(copyfrom_revision);
	const writerelated_node_baton *dir = (writerelated_node_baton*)parent_baton;

	writerelated_node_baton *child = (writerelated_node_baton*)apr_pcalloc(dir_pool, sizeof(writerelated_node_baton));
	child->client_baton = dir->client_baton;
	child->parent = dir;
	child->path = path;

	*child_baton = child;
	return nullptr;
}

static svn_error_t * __cdecl
writerelated_open_directory(const char *path, void *parent_baton, svn_revnum_t base_revision, apr_pool_t *dir_pool, void **child_baton)
{
	UNUSED_ALWAYS(base_revision);
	const writerelated_node_baton *dir = (writerelated_node_baton*)parent_baton;

	writerelated_node_baton *child = (writerelated_node_baton*)apr_pcalloc(dir_pool, sizeof(writerelated_node_baton));
	child->client_baton = dir->client_baton;
	child->parent = dir;
	child->path = path;

	*child_baton = child;
	return nullptr;
}

static svn_error_t * __cdecl
writerelated_change_dir_prop(void *dir_baton, const char *name, const svn_string_t *value, apr_pool_t *pool)
{
	UNUSED_ALWAYS(name);
	UNUSED_ALWAYS(value);
	UNUSED_ALWAYS(pool);
	const writerelated_node_baton *dir = (writerelated_node_baton*)dir_baton;

	return nullptr;
}

static svn_error_t * __cdecl
writerelated_close_directory(void *dir_baton, apr_pool_t *pool)
{
	UNUSED_ALWAYS(pool);
	const writerelated_node_baton *dir = (writerelated_node_baton*)dir_baton;
	return nullptr;
}

static svn_error_t * __cdecl
writerelated_absent_directory(const char *path, void *parent_baton, apr_pool_t *pool)
{
	UNUSED_ALWAYS(path);
	UNUSED_ALWAYS(pool);
	const writerelated_node_baton *dir = (writerelated_node_baton*)parent_baton;
	return nullptr;
}

static svn_error_t * __cdecl
writerelated_add_file(const char *path, void *parent_baton, const char *copyfrom_path, svn_revnum_t copyfrom_revision, apr_pool_t *file_pool, void **file_baton)
{
	UNUSED_ALWAYS(copyfrom_path);
	UNUSED_ALWAYS(copyfrom_revision);
	const writerelated_node_baton *dir = (writerelated_node_baton*)parent_baton;

	writerelated_node_baton *child = (writerelated_node_baton*)apr_pcalloc(file_pool, sizeof(writerelated_node_baton));
	child->client_baton = dir->client_baton;
	child->parent = dir;
	child->path = path;

	*file_baton = child;
	return nullptr;
}

static svn_error_t * __cdecl
writerelated_open_file(const char *path, void *parent_baton, svn_revnum_t base_revision, apr_pool_t *file_pool, void **file_baton)
{
	const writerelated_node_baton *dir = (writerelated_node_baton*)parent_baton;

	writerelated_node_baton *child = (writerelated_node_baton*)apr_pcalloc(file_pool, sizeof(writerelated_node_baton));
	child->client_baton = dir->client_baton;
	child->parent = dir;
	child->path = path;

	*file_baton = child;
	return nullptr;
}

static svn_error_t * __cdecl
writerelated_apply_textdelta(void *file_baton, const char *base_checksum, apr_pool_t *pool, svn_txdelta_window_handler_t *handler, void **handler_baton)
{
	const writerelated_node_baton *file = (writerelated_node_baton*)file_baton;

	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)file->client_baton);	

	SvnWriteRelatedArgs^ args = dynamic_cast<SvnWriteRelatedArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs
	if (!args)
		return nullptr;

	*handler_baton = nullptr;
	*handler = svn_delta_noop_window_handler;
	return nullptr;
}

static svn_error_t * __cdecl
writerelated_apply_change_file_prop(void *file_baton, const char *name, const svn_string_t *value, apr_pool_t *pool)
{
	const writerelated_node_baton *file = (writerelated_node_baton*)file_baton;

	return nullptr;
}

static svn_error_t * __cdecl
writerelated_close_file(void *file_baton, const char *text_checksum, apr_pool_t *pool)
{
	const writerelated_node_baton *file = (writerelated_node_baton*)file_baton;

	return nullptr;
}

static svn_error_t * __cdecl
writerelated_absent_file(const char *path, void *parent_baton, apr_pool_t *pool)
{
	const writerelated_node_baton *file = (writerelated_node_baton*)parent_baton;

	return nullptr;
}

static svn_error_t * __cdecl
writerelated_close_edit(void *edit_baton, apr_pool_t *pool)
{
	UNUSED_ALWAYS(edit_baton);
	UNUSED_ALWAYS(pool);
	return nullptr;
}

static svn_error_t * __cdecl
writerelated_abort_edit(void *edit_baton, apr_pool_t *pool)
{
	UNUSED_ALWAYS(edit_baton);
	UNUSED_ALWAYS(pool);
	return nullptr;
}

bool SvnClient::WriteRelated(ICollection<SvnUriTarget^>^ targets, ICollection<Stream^>^ to, SvnWriteRelatedArgs^ args)
{
	if (!targets)
		throw gcnew ArgumentNullException("targets");
	else if (!to)
		throw gcnew ArgumentNullException("to");
	else if (!args)
		throw gcnew ArgumentNullException("args");
	else if (!targets->Count)
		throw gcnew ArgumentException(SharpSvnStrings::CollectionMustContainAtLeastOneItem, "targets");
	else if (targets->Count != to->Count)
		throw gcnew ArgumentException("Collection lengths must match", "targets");

	for each (SvnUriTarget^ t in targets)
	{
		if (!t)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targets");
	}

	for each (Stream^ t in to)
	{
		if (!t)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "to");
	}

	array<SvnUriTarget^>^ theTargets = gcnew array<SvnUriTarget^>(targets->Count);
	array<Stream^>^ theStreams = gcnew array<Stream^>(to->Count);

	targets->CopyTo(theTargets, 0);
	to->CopyTo(theStreams, 0);

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);
	AprPool^ cur = gcnew AprPool(%pool);
	AprPool^ next = gcnew AprPool(%pool);
	//if (versionHandler)
	//	args->FileVersion += versionHandler;
	try
	{
		svn_ra_session_t* ra_session = nullptr;		

		const char* baseUrl = nullptr;
		svn_revnum_t baseRev;

		const char* pUrl = nullptr;
		svn_error_t* r;
		svn_revnum_t end_rev;
		svn_revnum_t rev_head;

		// if (args->WcBase)
		// {
		// }
		// else
		{
			SvnUriTarget^ first = FirstOf(targets);
			const char* pTarget = pool.AllocString(first->SvnTargetName);

			r = svn_client__ra_session_from_path(
				&ra_session, 
				&end_rev,
				&pUrl,
				pTarget,			
				nullptr, 
				first->Revision->AllocSvnRevision(%pool),
				first->Revision->AllocSvnRevision(%pool), 
				CtxHandle, 
				pool.Handle);

			if (r)
				return args->HandleResult(this, r);

			// Get the single revision to use as head for the next calls
			r = svn_ra_get_latest_revnum(ra_session, &rev_head, pool.Handle);

			if (r)
				return args->HandleResult(this, r);

			baseUrl = pUrl;
			baseRev = 0; // Nothing exists
		}

		svn_delta_editor_t* editor = svn_delta_default_editor(pool.Handle);
		editor->set_target_revision = writerelated_set_target_revision;
		editor->open_root = writerelated_open_root;
		editor->delete_entry = writerelated_delete_entry;
		editor->add_directory = writerelated_add_directory;
		editor->open_directory = writerelated_open_directory;
		editor->change_dir_prop = writerelated_change_dir_prop;
		editor->close_directory = writerelated_close_directory;
		editor->absent_directory = writerelated_absent_directory;
		editor->add_file = writerelated_add_file;
		editor->open_file = writerelated_open_file;
		editor->apply_textdelta = writerelated_apply_textdelta;
		editor->change_file_prop = writerelated_apply_change_file_prop;
		editor->close_file = writerelated_close_file;
		editor->absent_file = writerelated_absent_file;
		editor->close_edit = writerelated_close_edit;
		editor->abort_edit = writerelated_abort_edit;

		// editor->change_file_prop = 
		// editor->apply_textdelta = 

		for each(SvnUriTarget^ ta in targets)
		{
			cur->Clear();

			svn_revnum_t switchToRevision = (svn_revnum_t)ta->Revision->Revision;
			const char* switchUrl = cur->AllocString(ta->SvnTargetName);

			const char* baseUrlDir;
			const char* baseUrlFile;
			svn_path_split(baseUrl, &baseUrlDir, &baseUrlFile, cur->Handle);

			const char* switchUrlDir;
			const char* switchUrlFile;
			svn_path_split(switchUrl, &switchUrlDir, &switchUrlFile, cur->Handle);

			r = svn_ra_reparent(ra_session, baseUrlDir, cur->Handle);

			if (r)
				return args->HandleResult(this, r);

			const svn_ra_reporter3_t* reporter = nullptr;
			void* report_baton = nullptr;

			r = svn_ra_do_switch2(ra_session, &reporter, &report_baton,
				switchToRevision,
				baseUrlFile,
				svn_depth_empty,
				switchUrl,
				editor,
				(void*)_clientBatton->Handle, cur->Handle);

			if (r)
				return args->HandleResult(this, r);

			r = reporter->set_path(
					report_baton,
					"",
					baseRev,
					svn_depth_empty,
					false,
					nullptr,
					cur->Handle);

			if (r)
				return args->HandleResult(this, r);

/*			r = reporter->set_path(
					report_baton,
					baseUrlFile,
					baseRev,
					svn_depth_empty,
					false, // Start empty
					nullptr,
					cur->Handle);

			if (r)
				return args->HandleResult(this, r);*/

			// Close the reporter, this will start the editor walk
			r = reporter->finish_report(report_baton, cur->Handle);

			if (r)
				return args->HandleResult(this, r);

			baseUrl = switchUrl;
			baseRev = switchToRevision;

			{
				// Swap pools for next action
				AprPool^ tmp = next;
				next = cur;
				cur = tmp;			
			}
		}


		return true;
	}
	finally
	{
	}
}

#endif