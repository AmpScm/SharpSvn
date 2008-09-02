// $Id: FileVersions.cpp 704 2008-08-17 19:45:19Z rhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"
#include "Args/GetCapabilities.h"
#include <svn_ra.h>

#include "UnmanagedStructs.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Collections::Generic;

bool SvnClient::GetCapabilities(SvnTarget^ target, IEnumerable<SvnCapability>^ retrieve, [Out]Collection<SvnCapability>^% capabilities)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!retrieve)
		throw gcnew ArgumentNullException("retrieve");

	SvnGetCapabilitiesArgs^ args = gcnew SvnGetCapabilitiesArgs();
	for each (SvnCapability c in retrieve)
	{
		if (!args->Capabilities->Contains(c))
			args->Capabilities->Add(c);
	}

	return GetCapabilities(target, args, capabilities);
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
#include "../../../imports/build/win32/subversion/subversion/libsvn_client/client.h"

bool SvnClient::GetCapabilities(SvnTarget^ target, SvnGetCapabilitiesArgs^ args, [Out]Collection<SvnCapability>^% capabilities)
{
	if (!target)
		throw gcnew ArgumentNullException("target");
	else if (!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_ra_session_t* ra_session = nullptr;
	const char* pTarget = pool.AllocString(target->SvnTargetName);
	const char* pUrl = nullptr;
	svn_revnum_t end_rev = 0;

	svn_error_t* r;

	r = svn_client__ra_session_from_path(
		&ra_session, 
		&end_rev,
		&pUrl,
		pTarget,			
		nullptr, 
		target->Revision->AllocSvnRevision(%pool),
		target->Revision->AllocSvnRevision(%pool), 
		CtxHandle, 
		pool.Handle);

	if (r)
		return args->HandleResult(this, r);
	capabilities = gcnew Collection<SvnCapability>();
	AprPool^ tmp = gcnew AprPool(%pool);
	for each(SvnCapability c in args->RetrieveAllCapabilities ? static_cast<System::Collections::IEnumerable^>(Enum::GetValues(SvnCapability::typeid)) : args->Capabilities)
	{
		const char* cap = nullptr;
		switch(c)
		{
		case SvnCapability::Depth:
			cap = SVN_RA_CAPABILITY_DEPTH;
			break;
		case SvnCapability::MergeInfo:
			cap = SVN_RA_CAPABILITY_MERGEINFO;
			break;
		case SvnCapability::LogRevisionProperties:
			cap = SVN_RA_CAPABILITY_LOG_REVPROPS;
			break;
		case SvnCapability::PartialReplay:
			cap = SVN_RA_CAPABILITY_PARTIAL_REPLAY;
			break;
		case SvnCapability::CommitRevisionProperties:
			cap = SVN_RA_CAPABILITY_COMMIT_REVPROPS;
			break;
		case SvnCapability::None:
			if (args->RetrieveAllCapabilities)
				continue;
		default:
			throw gcnew ArgumentOutOfRangeException("args", "Invalid capability specified");
		}

		if(capabilities->Contains(c))
			continue;

		svn_boolean_t has = 0;

		r = svn_ra_has_capability(ra_session, &has, cap, tmp->Handle);

		if (r && r->apr_err == SVN_ERR_UNKNOWN_CAPABILITY)
		{
			// Some RA layers leave it to the server what they support or not. 
			// Just eat this error to have some compatibility over different versions
			svn_error_clear(r);
			r = nullptr;
		}
		else if (r)
			return args->HandleResult(this, r);
		else 

		if(has)
		{
			capabilities->Add(c);
		}
	}

	return args->HandleResult(this, r);
}

