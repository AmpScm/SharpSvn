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
#include "Delta/SvnDeltaEditor.h"
#include "Args/ReplayRevision.h"

#include "svn_ra.h"
#include "svn-internal/libsvn_client/client.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace SharpSvn::Delta;
using namespace System::Collections::Generic;


bool SvnClient::ReplayRevisions(SvnTarget^ target, SvnRevisionRange^ range, Delta::SvnDeltaEditor^ editor)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!range)
		throw gcnew ArgumentNullException("range");
	else if (!editor)
		throw gcnew ArgumentNullException("editor");

	return ReplayRevisions(target, range, editor, gcnew SvnReplayRevisionArgs());
}

static svn_error_t* __cdecl
sharpsvn_replay_rev_start(svn_revnum_t revision, void *replay_baton, const svn_delta_editor_t **editor,
						  void **edit_baton, apr_hash_t *rev_props, apr_pool_t *pool)
{
	SvnClient^ client = AprBaton<SvnClient^>::Get((IntPtr)replay_baton);

	AprPool thePool(pool, false);

	SvnReplayRevisionArgs^ args = dynamic_cast<SvnReplayRevisionArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnReplayRevisionArgs
	if (!args)
		return nullptr;

	SvnReplayRevisionStartEventArgs^ a = gcnew SvnReplayRevisionStartEventArgs(revision, args->_deltaEditor, rev_props, %thePool);

	try
	{
		args->InvokeRevisionStart(a);

		if (a->Cancel)
			return svn_error_create (SVN_ERR_CANCELLED, nullptr, "Operation canceled");

		svn_delta_editor_t *innerEditor;
		void* innerBaton = nullptr;
		if (a->Editor)
		{
			a->Editor->AllocEditor(client, editor, edit_baton, %thePool);

			return nullptr;
		}
		else
		{
			innerEditor = svn_delta_default_editor(pool);

			return svn_delta_get_cancellation_editor(
				client->CtxHandle->cancel_func,
				client->CtxHandle->cancel_baton,
				innerEditor,
				innerBaton,
				editor,
				edit_baton,
				pool);
		}
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

	SvnReplayRevisionArgs^ args = dynamic_cast<SvnReplayRevisionArgs^>(client->CurrentCommandArgs); // C#: _currentArgs as SvnReplayRevisionArgs
	if (!args)
		return nullptr;

	SvnReplayRevisionEndEventArgs^ a = gcnew SvnReplayRevisionEndEventArgs(revision, rev_props, %thePool);

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

bool SvnClient::ReplayRevisions(SvnTarget^ target, SvnRevisionRange^ range, Delta::SvnDeltaEditor^ editor, SvnReplayRevisionArgs^ args)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!range)
		throw gcnew ArgumentNullException("range");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	// Allow null editor! (We allow overriding from args)

	EnsureState(SvnContextState::AuthorizationInitialized);
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);

	try
	{
		svn_ra_session_t* ra_session = nullptr;
		const char* pTarget = target->AllocAsString(%pool);
		svn_client__pathrev_t *pathrev;
		svn_revnum_t start_rev = 0;
		svn_revnum_t watermark_rev = 0;

		svn_error_t* r;

		r = svn_client__ra_session_from_path2(
			&ra_session,
			&pathrev,
			pTarget,
			nullptr,
			target->Revision->AllocSvnRevision(%pool),
			range->EndRevision->AllocSvnRevision(%pool),
			CtxHandle,
			pool.Handle);

		if (r)
			return args->HandleResult(this, r, target);

		// <CancelChecking> // We replace the client layer here; we must check for cancel
		SvnCancelEventArgs^ cA = gcnew SvnCancelEventArgs();

		HandleClientCancel(cA);

		if(cA->Cancel)
			return args->HandleResult(this, gcnew SvnOperationCanceledException("Operation Canceled"), target);

		r = svn_client__get_revision_number(
			&start_rev,
			nullptr,
			CtxHandle->wc_ctx,
			pTarget,
			ra_session,
			range->StartRevision->AllocSvnRevision(%pool),
			pool.Handle);

		if (r)
			return args->HandleResult(this, r, target);

		r = svn_client__get_revision_number(
			&watermark_rev,
			nullptr,
			CtxHandle->wc_ctx,
			pTarget,
			ra_session,
			args->LowWatermarkRevision->AllocSvnRevision(%pool),
			
			pool.Handle);

		if (r)
			return args->HandleResult(this, r, target);

		if (start_rev == 0 && pathrev->rev > start_rev)
		{
			// Replaying revision 0 (which contains nothing per definition)
			// Breaks the watermark handling in subversion. This is currently
			// only visible with RetrieveContent set

			start_rev = 1;
		}

		args->_deltaEditor = editor;

		r = svn_ra_replay_range(
			ra_session,
			start_rev,
			pathrev->rev,
			watermark_rev,
			args->RetrieveContent,
			sharpsvn_replay_rev_start,
			sharpsvn_replay_rev_finish,
			(void*)_clientBaton->Handle,
			pool.Handle);

		return args->HandleResult(this, r, target);
	}
	finally
	{
		args->_deltaEditor = nullptr;
	}

	return false;
}
