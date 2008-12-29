// $Id: RemoveFromChangeList.cpp 623 2008-07-23 22:10:29Z rhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Delta/SvnDeltaEditor.h"
#include "Args/Replay.h"

#include "svn_ra.h"
#include "svn-internal/libsvn_client/client.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace SharpSvn::Delta;
using namespace System::Collections::Generic;


bool SvnClient::Replay(SvnTarget^ target, SvnRevisionRange^ range, Delta::SvnDeltaEditor^ editor)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!range)
		throw gcnew ArgumentNullException("range");
	else if (!editor)
		throw gcnew ArgumentNullException("editor");

	return Replay(target, range, editor, gcnew SvnReplayArgs());
}

static svn_error_t* __cdecl
sharpsvn_replay_rev_start(svn_revnum_t revision, void *replay_baton, const svn_delta_editor_t **editor,
						  void **edit_baton, apr_hash_t *rev_props, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)replay_baton);	

	AprPool thePool(pool, false);

	SvnReplayArgs^ args = dynamic_cast<SvnReplayArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnReplayArgs
	if (!args)
		return nullptr;

	SvnReplayRevisionStartEventArgs^ a = gcnew SvnReplayRevisionStartEventArgs(revision, args->_deltaEditor, rev_props);

	try
	{
		args->InvokeRevisionStart(a);

		if (a->Cancel)
			return svn_error_create (SVN_ERR_CANCELLED, nullptr, "Operation canceled");

		svn_delta_editor_t *innerEditor;
		void* innerBaton = nullptr;
		if (a->Editor)
		{
			innerEditor = a->Editor->AllocEditor(&innerBaton, %thePool);
		}
		else
		{
			innerEditor = svn_delta_default_editor(pool);
		}

		return svn_delta_get_cancellation_editor(
			client->CtxHandle->cancel_func,
			client->CtxHandle->cancel_baton,
			innerEditor,
			innerBaton,
			editor,
			edit_baton,
			pool);
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("replay_rev_start", e);
	}
	finally
	{
		a->Detach(false);
	}
}

static svn_error_t* __cdecl
sharpsvn_replay_rev_finish(svn_revnum_t revision, void *replay_baton, const svn_delta_editor_t *editor,
							 void *edit_baton, apr_hash_t *rev_props, apr_pool_t *pool)
{
	UNUSED_ALWAYS(editor);
	UNUSED_ALWAYS(edit_baton);
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)replay_baton);	

	AprPool thePool(pool, false);

	SvnReplayArgs^ args = dynamic_cast<SvnReplayArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnReplayArgs
	if (!args)
		return nullptr;
	
	SvnReplayRevisionEndEventArgs^ a = gcnew SvnReplayRevisionEndEventArgs(revision, rev_props);

	try
	{
		args->InvokeRevisionEnd(a);

		if (a->Cancel)
			return svn_error_create (SVN_ERR_CANCELLED, nullptr, "Operation canceled");

		return nullptr;
	}
	catch (Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("replay_rev_finish", e);
	}
	finally
	{
		a->Detach(false);
	}
}

bool SvnClient::Replay(SvnTarget^ target, SvnRevisionRange^ range, Delta::SvnDeltaEditor^ editor, SvnReplayArgs^ args)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!range)
		throw gcnew ArgumentNullException("range");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	// Allow null editor! (We allow overriding from args)

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	try
	{
		svn_ra_session_t* ra_session = nullptr;
		const char* pTarget = pool.AllocString(target->SvnTargetName);
		const char* pUrl = nullptr;
		const char* repos_root = nullptr;
		svn_revnum_t end_rev = 0;
		svn_revnum_t start_rev = 0;
		svn_revnum_t watermark_rev = 0;

		svn_error_t* r;

		r = svn_client__ra_session_from_path(
			&ra_session, 
			&end_rev,
			&pUrl,
			pTarget,			
			nullptr, 
			target->Revision->AllocSvnRevision(%pool),
			range->EndRevision->AllocSvnRevision(%pool), 
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
			range->StartRevision->AllocSvnRevision(%pool),
			pTarget, 
			pool.Handle);

		if (r)
			return args->HandleResult(this, r);

		r = svn_client__get_revision_number(
			&watermark_rev,
			nullptr,
			ra_session,
			args->LowWaterMarkRevision->AllocSvnRevision(%pool),
			pTarget, 
			pool.Handle);

		if (r)
			return args->HandleResult(this, r);

		
		r = svn_ra_get_repos_root2(ra_session, &repos_root, pool.Handle);

		if (r)
			return args->HandleResult(this, r);

		args->_deltaEditor = editor;

		r = svn_ra_replay_range(
			ra_session,
			start_rev,
			end_rev,
			watermark_rev,
			args->RetrieveContent,
			sharpsvn_replay_rev_start,
			sharpsvn_replay_rev_finish,
			(void*)_clientBatton->Handle,
			pool.Handle);

		return args->HandleResult(this, r);
	}
	finally
	{
		args->_deltaEditor = nullptr;
	}

	return false;
}
