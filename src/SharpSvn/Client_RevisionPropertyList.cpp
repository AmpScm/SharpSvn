#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;


void SvnClient::RevisionPropertyList(SvnUriTarget^ target, EventHandler<SvnRevisionPropertyListEventArgs^>^ listHandler)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!listHandler)
		throw gcnew ArgumentNullException("listHandler");

	RevisionPropertyList(target, gcnew SvnRevisionPropertyListArgs(), listHandler);
}

bool SvnClient::RevisionPropertyList(SvnUriTarget^ target, SvnRevisionPropertyListArgs^ args, EventHandler<SvnRevisionPropertyListEventArgs^>^ listHandler)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	// We allow a null listHandler; the args object might just handle it itself

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	if(listHandler)
		args->PropertyList += listHandler;
	try
	{
		apr_hash_t *props = nullptr;
		svn_revnum_t set_rev = 0;

		// This method will probably get a callback structure in the near (SVN 1.5) future; we just fake it for now

		svn_error_t* err = svn_client_revprop_list(
			&props,
			pool.AllocString(target->ToString()),
			target->Revision->AllocSvnRevision(%pool),
			&set_rev,			
			CtxHandle,
			pool.Handle);

		if(!err && props)
		{
			SvnRevisionPropertyListEventArgs^ a = gcnew SvnRevisionPropertyListEventArgs(props, %pool);

			args->OnPropertyList(a);

			a->Detach(false);
		}

		return args->HandleResult(err);
	}
	finally
	{
		if(listHandler)
			args->PropertyList -= listHandler;
	}
}

void SvnClient::GetRevisionPropertyList(SvnUriTarget^ target, [Out] IDictionary<String^, Object^>^% list)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	InfoItemCollection<SvnRevisionPropertyListEventArgs^>^ results = gcnew InfoItemCollection<SvnRevisionPropertyListEventArgs^>();

	try
	{
		RevisionPropertyList(target, gcnew SvnRevisionPropertyListArgs(), results->Handler);
	}
	finally
	{
		if(results->Count > 0)
			list = results[0]->Properties;
		else
			list = nullptr;
	}
}

bool SvnClient::GetRevisionPropertyList(SvnUriTarget^ target, SvnRevisionPropertyListArgs^ args, [Out] IDictionary<String^, Object^>^% list)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	InfoItemCollection<SvnRevisionPropertyListEventArgs^>^ results = gcnew InfoItemCollection<SvnRevisionPropertyListEventArgs^>();

	try
	{
		return RevisionPropertyList(target, args, results->Handler);
	}
	finally
	{
		if(results->Count > 0)
			list = results[0]->Properties;
		else
			list = nullptr;
	}
}
