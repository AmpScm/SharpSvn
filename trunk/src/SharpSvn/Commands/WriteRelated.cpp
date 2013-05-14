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
#include "SvnStreamWrapper.h"
#include "Args/WriteRelated.h"
#include "Delta/SvnDeltaEditor.h"
#include "svn-internal/libsvn_client/client.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using System::Collections::Generic::IDictionary;
using namespace SharpSvn;
using namespace SharpSvn::Delta;
using namespace SharpSvn::Implementation;
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

ref class WriteRelatedEditor : Delta::SvnDeltaEditor
{
protected public:
	virtual void OnSetTarget(SvnDeltaSetTargetEventArgs^ e) override
	{
		__super::OnSetTarget(e);
	}

	virtual void OnFilePropertyChange(SvnDeltaFilePropertyChangeEventArgs^ e) override
	{
		__super::OnFilePropertyChange(e);
	}

	virtual void OnFileChange(SvnDeltaFileChangeEventArgs^ e) override
	{
		__super::OnFileChange(e);
	}
};

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
	AprPool pool(%_pool);
	ArgsStore store(this, args, %pool);
	AprPool^ cur = gcnew AprPool(%pool);
	AprPool^ next = gcnew AprPool(%pool);
	WriteRelatedEditor^ wrEditor = gcnew WriteRelatedEditor();
	//if (versionHandler)
	//	args->FileVersion += versionHandler;
	try
	{
		svn_ra_session_t* ra_session = nullptr;

		const char* baseUrl = nullptr;
		svn_revnum_t baseRev;

		svn_client__pathrev_t *pathrev;
		svn_error_t* r;
		svn_revnum_t rev_head;

		// if (args->WcBase)
		// {
		// }
		// else
		{
			SvnUriTarget^ first = FirstOf(targets);
			const char* pTarget = first->AllocAsString(%pool);

			r = svn_client__ra_session_from_path2(
				&ra_session,
				&pathrev,
				pTarget,
				nullptr,
				first->Revision->AllocSvnRevision(%pool),
				first->Revision->AllocSvnRevision(%pool),
				CtxHandle,
				pool.Handle);

			if (r)
				return args->HandleResult(this, r, targets);

			// Get the single revision to use as head for the next calls
			r = svn_ra_get_latest_revnum(ra_session, &rev_head, pool.Handle);

			if (r)
				return args->HandleResult(this, r, targets);

			baseUrl = pathrev->url;
			baseRev = 0; // Nothing exists
		}

		const svn_delta_editor_t* editor;
		void* editor_baton;
		wrEditor->AllocEditor(this, &editor, &editor_baton, %pool);

		for each(SvnUriTarget^ ta in targets)
		{
			cur->Clear();

			svn_revnum_t switchToRevision = (svn_revnum_t)ta->Revision->Revision;
			const char* switchUrl = ta->AllocAsString(cur);

			const char* baseUrlDir;
			const char* baseUrlFile;
			svn_uri_split(&baseUrlDir, &baseUrlFile, baseUrl, cur->Handle);

			const char* switchUrlDir;
			const char* switchUrlFile;
			svn_uri_split(&switchUrlDir, &switchUrlFile, switchUrl, cur->Handle);

			r = svn_ra_reparent(ra_session, baseUrlDir, cur->Handle);

			if (r)
				return args->HandleResult(this, r, targets);

			const svn_ra_reporter3_t* reporter = nullptr;
			void* report_baton = nullptr;

			r = svn_ra_do_switch2(ra_session, &reporter, &report_baton,
				switchToRevision,
				baseUrlFile,
				svn_depth_empty,
				switchUrl,
				editor,
				editor_baton,
				cur->Handle);

			if (r)
				return args->HandleResult(this, r, targets);

			r = reporter->set_path(
					report_baton,
					"",
					baseRev,
					svn_depth_empty,
					false,
					nullptr,
					cur->Handle);

			if (r)
				return args->HandleResult(this, r, targets);

			// Close the reporter, this will start the editor walk
			r = reporter->finish_report(report_baton, cur->Handle);

			if (r)
				return args->HandleResult(this, r, targets);

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