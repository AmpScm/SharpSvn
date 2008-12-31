// $Id: Write.cpp 910 2008-11-28 00:14:38Z rhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

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
public:
	virtual void OnSetTarget(SvnDeltaSetTargetEventArgs^ e) override
	{
		__super::OnSetTarget(e);
	}

	virtual void OnFilePropertyChange(SvnDeltaFilePropertyChangeEventArgs^ e) override
	{
		__super::OnFilePropertyChange(e);
	}

	virtual void OnBeforeFileDelta(SvnDeltaBeforeFileDeltaEventArgs^ e) override
	{
		__super::OnBeforeFileDelta(e);
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
	ArgsStore store(this, args);
	AprPool pool(%_pool);
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

		const svn_delta_editor_t* editor;
		void* editor_baton;
		wrEditor->AllocEditor(this, &editor, &editor_baton, %pool);

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
				editor_baton, 
				cur->Handle);

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