// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/FileVersions.h"
#include "SvnStreamWrapper.h"

#include <svn_ra.h>
#include <svn_subst.h>
#include "UnmanagedStructs.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;
using System::IO::Stream;

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

	SvnFileVersionEventArgs^ e = nullptr;
	try
	{
		SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)dbaton->clientBaton);	
		if (window)
		{
			SvnCancelEventArgs^ ca = gcnew SvnCancelEventArgs();

			client->HandleClientCancel(ca);

			if (ca->Cancel)
				return svn_error_create (SVN_ERR_CANCELLED, nullptr, "Operation canceled");


			return nullptr; // We are only interested after the file is complete
		}

		SvnFileVersionsArgs^ args = dynamic_cast<SvnFileVersionsArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs
		if (!args)
			return nullptr;	

		AprPool^ next = args->_prevPool;

		if (dbaton->source_file)
			svn_io_file_close(dbaton->source_file, next->Handle);

		// Clean up for the next round
		args->_lastFile = dbaton->filename;
		args->_prevPool = args->_curPool;
		args->_curPool = next;

		e = args->_fv;	
	
		args->_fv = nullptr;
		if (args->RetrieveContents && e)
		{
			e->SetPool(next);
			args->OnFileVersion(e);
		}

		next->Clear();
	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("FileVersions delta window receiver", e);
	}
	finally
	{
		if (e)
			e->Detach(false);
	}	

	return nullptr;
}

#pragma warning(disable: 4127) // conditional expression is constant on SVN_ERR

/* Apply property changes on the current property state and copy everything to the new pool
* to make sure the properties are still valid in the next round
*/
static svn_error_t*
apply_property_changes(SvnFileVersionsArgs^ args, apr_array_header_t *props, AprPool^ allocPool, bool *property_changes, AprPool^ tmpPool)
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

			*property_changes = true;

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

	bool property_changes = false;
	// <Update Property List>
	SVN_ERR(apply_property_changes(args, prop_diffs, args->_curPool, &property_changes, %thePool));
	// </Update Property List>

	AprPool handlerPool(%thePool);


	SvnFileVersionEventArgs^ e = gcnew SvnFileVersionEventArgs(
		args->_reposRoot,
		path, 
		rev,
		rev_props, 
		args->RetrieveProperties ? args->_properties : nullptr,
		(content_delta_handler && content_delta_baton && args->RetrieveContents),
		property_changes,
		result_of_merge!=0, 
		client, 							
		%handlerPool);

	bool detach = true;
	bool nodetach = false;
	try
	{
		// <CancelChecking> 
		client->HandleClientCancel(cA);

		if(cA->Cancel)
			return svn_error_create (SVN_ERR_CANCELLED, nullptr, "Operation canceled");
		// </CancelChecking>        

		if (e->HasContentChanges)
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
			
			AprPool^ filePool;

			if (args->_curFilePool && !result_of_merge)
				filePool = args->_curFilePool;
			else
				filePool = args->_curPool;

			SVN_ERR(svn_io_open_unique_file2(&delta_baton->file,
				&delta_baton->filename,
				args->_tempPath,
				"SvnFV", svn_io_file_del_on_pool_cleanup,
				filePool->Handle));

			svn_stream_t* last_stream = svn_stream_from_aprfile2(delta_baton->source_file, false, _pool->Handle);
			svn_stream_t* cur_stream = svn_stream_from_aprfile2(delta_baton->file, false, _pool->Handle);

			/* Get window handler for applying delta. */
			svn_txdelta_apply(last_stream, cur_stream, NULL, NULL,
				_pool->Handle,
				&delta_baton->wrapped_handler,
				&delta_baton->wrapped_baton);

			/* Wrap the window handler with our own. */
			*content_delta_handler = file_version_window_handler;
			*content_delta_baton = delta_baton;

			args->_fv = e;
			nodetach = true;
		}
		else
			args->OnFileVersion(e);		

		if (e->Cancel)
			return svn_error_create(SVN_ERR_CEASE_INVOCATION, nullptr, "Version receiver canceled operation");

		// <CancelChecking> 
		client->HandleClientCancel(cA);

		if(cA->Cancel)
			return svn_error_create (SVN_ERR_CANCELLED, nullptr, "Operation canceled");
		// </CancelChecking>

		if(nodetach)
			detach = false;

		return nullptr;
	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("FileVersions receiver", e);
	}
	finally
	{
		if(detach)
			e->Detach(false);
		else
		{
			// Clone to prevpool, as that is the first to be disposed
			e->DetachBeforeFileData(args->_prevPool); 
		}
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
		const char* repos_root = nullptr;
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

		// <CancelChecking> // We replace the client layer here; we must check for cancel
		SvnCancelEventArgs^ cA = gcnew SvnCancelEventArgs();

		HandleClientCancel(cA);

		if(cA->Cancel)
			return args->HandleResult(this, gcnew SvnOperationCanceledException("Operation Canceled"));

		r = svn_client__get_revision_number(
			&start_rev,
			nullptr,
			ra_session,
			args->Start->AllocSvnRevision(%pool),
			pTarget, 
			pool.Handle);

		if (r)
			return args->HandleResult(this, r);
		
		r = svn_ra_get_repos_root2(ra_session, &repos_root, pool.Handle);

		if (r)
			return args->HandleResult(this, r);

		args->_keepPool = gcnew AprPool(%pool);
		args->_curPool = gcnew AprPool(%pool);
		args->_prevPool = gcnew AprPool(%pool);
		args->_curFilePool = nullptr;
		args->_prevFilePool = nullptr;
		args->_tempPath = pool.AllocPath(System::IO::Path::Combine(System::IO::Path::GetTempPath(), "SvnFv"));
		args->_lastFile = nullptr;
		args->_properties = nullptr;
		args->_reposRoot = repos_root;

		if (args->RetrieveMergedRevisions)
		{
			args->_curFilePool = gcnew AprPool(%pool);
			args->_prevFilePool = gcnew AprPool(%pool);
		}		

		r = svn_ra_get_file_revs2(
			ra_session, 
			"", // We opened the repository at the right spot
			start_rev,
			end_rev, 
			args->RetrieveMergedRevisions,
			file_version_handler,
			(void*)_clientBatton->Handle, 
			pool.Handle);

		return args->HandleResult(this, r);
	}
	finally
	{
		args->_keepPool = nullptr;
		args->_curPool = nullptr;
		args->_prevPool = nullptr;
		args->_curFilePool = nullptr;
		args->_prevFilePool = nullptr;

		args->_lastFile = nullptr;
		args->_tempPath = nullptr;
		args->_reposRoot = nullptr;
		args->_properties = nullptr;
		args->_curKwProps = nullptr;
		args->_fv = nullptr;

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

void SvnFileVersionEventArgs::WriteTo(String^ outputFileName)
{
	if (String::IsNullOrEmpty(outputFileName))
		throw gcnew ArgumentNullException("outputFileName");
	else if (!SvnBase::IsNotUri(outputFileName))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "outputFileName");

	WriteTo(outputFileName, gcnew SvnFileVersionWriteArgs());	
}

void SvnFileVersionEventArgs::WriteTo(System::IO::Stream^ output)
{
	if (!output)
		throw gcnew ArgumentNullException("output");

	WriteTo(output, gcnew SvnFileVersionWriteArgs());
}

void SvnFileVersionEventArgs::WriteTo(String^ outputFileName, SvnFileVersionWriteArgs^ args)
{
	if (String::IsNullOrEmpty(outputFileName))
		throw gcnew ArgumentNullException("outputFileName");
	else if (!SvnBase::IsNotUri(outputFileName))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "outputFileName");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	bool error = false;
	try
	{
		System::IO::FileStream to(outputFileName, System::IO::FileMode::Create);
		error = true;

		WriteTo(%to, args);

		error = false;
	}
	finally
	{
		if (error)
			System::IO::File::Delete(outputFileName);
	}
}

void SvnFileVersionEventArgs::WriteTo(System::IO::Stream^ output, SvnFileVersionWriteArgs^ args)
{
	if (!output)
		throw gcnew ArgumentNullException("output");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	// The easy way out, just use the content stream
	Stream^ from = GetContentStream(args);
	try
	{
		array<Byte>^ buffer = gcnew array<Byte>(4096);

		int n;

		while (0 < (n = from->Read(buffer, 0, buffer->Length)))
		{
			output->Write(buffer, 0, n);
		}
	}
	finally
	{
		from->Close();
	}
}

Stream^ SvnFileVersionEventArgs::GetContentStream()
{
	return GetContentStream(gcnew SvnFileVersionWriteArgs());
}

Stream^ SvnFileVersionEventArgs::GetContentStream(SvnFileVersionWriteArgs^ args)
{
	if (!args)
		throw gcnew ArgumentNullException("args");

	SvnClient^ client = _client;

	if (!client || !_pool)
		throw gcnew InvalidOperationException("This method can only be invoked from the eventhandler handling this eventargs instance");

	SvnFileVersionsArgs^ fvArgs = dynamic_cast<SvnFileVersionsArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnCommitArgs

	if (!fvArgs)
		throw gcnew InvalidOperationException("This method can only be invoked when the client is still handling this request");

	apr_file_t* txt;
	svn_error_t* err = svn_io_file_open(&txt, fvArgs->_lastFile, APR_READ, APR_OS_DEFAULT, _pool->Handle);

	if (err)
		throw SvnException::Create(err);

	svn_stream_t* stream = svn_stream_from_aprfile2(txt, false, _pool->Handle);

	if (!args->WriteUntranslated)
	{
		SvnLineStyle ls = fvArgs->LineStyle;

		const char* eol = nullptr;
		if (ls == SvnLineStyle::Default)
		{
			svn_string_t* val = nullptr;
			
			if (_fileProps)
				val = (svn_string_t*)apr_hash_get(_fileProps, SVN_PROP_EOL_STYLE, APR_HASH_KEY_STRING);

			if(val)
			{
				svn_subst_eol_style style = svn_subst_eol_style_native;

				svn_subst_eol_style_from_value(&style, &eol, val->data);
			}
		}
		else
			eol = SvnClient::GetEolValue(ls);

		stream = svn_subst_stream_translated(
						stream,
						eol,
						fvArgs->RepairLineEndings,
						GetKeywords(fvArgs),
						fvArgs->KeywordExpansion != SvnKeywordExpansion::None,
						_pool->Handle);	
	}

	return gcnew Implementation::SvnWrappedStream(stream, _pool); // Inner stream is automatically closed on pool destruction
}

apr_hash_t* SvnFileVersionEventArgs::GetKeywords(SvnFileVersionsArgs^ args)
{
	bool keepValues = false;
	switch (args->KeywordExpansion)
	{
	case SvnKeywordExpansion::None:
		return nullptr;
	case SvnKeywordExpansion::SameValues:
		if (args->_curKwProps)
		{
			if (!apr_hash_count(args->_curKwProps))
				return nullptr; // Negative cache
			else
				return args->_curKwProps;
		}
		keepValues = true;
		break;
	case SvnKeywordExpansion::Default:
		break;
	default:
		throw gcnew InvalidOperationException();
	}

	svn_string_t* kwProp = nullptr;
	
	if (args->_properties)
		kwProp = (svn_string_t*)apr_hash_get(args->_properties, SVN_PROP_KEYWORDS, APR_HASH_KEY_STRING);

	if (!kwProp)
	{
		if (args->KeywordExpansion == SvnKeywordExpansion::SameValues)
			args->_curKwProps = apr_hash_make(args->_keepPool->Handle); // negative cache

		return nullptr;
	}

	AprPool^ pool = keepValues ? args->_keepPool : args->_curPool;

	apr_hash_t* kw = nullptr;

	svn_error_t* r = svn_subst_build_keywords2(
						&kw, 
						kwProp->data, 
						apr_psprintf(pool->Handle, "%ld", (svn_revnum_t)Revision),
						ItemUrl,
						SvnBase::AprTimeFromDateTime(Time),
						apr_pstrdup(pool->Handle, _pcAuthor),
						pool->Handle);

	if (r)
		throw SvnException::Create(r);

	if (keepValues)
		args->_curKwProps = kw;

	return kw;
}