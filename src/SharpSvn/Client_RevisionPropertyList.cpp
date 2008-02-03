// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;


[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetRevisionPropertyList(SharpSvn.SvnUriTarget,System.Collections.Generic.IDictionary`2<System.String,System.Object>&):System.Boolean", MessageId="1#")];
[module: SuppressMessage("Microsoft.Design", "CA1021:AvoidOutParameters", Scope="member", Target="SharpSvn.SvnClient.GetRevisionPropertyList(SharpSvn.SvnUriTarget,SharpSvn.SvnRevisionPropertyListArgs,System.Collections.Generic.IDictionary`2<System.String,System.Object>&):System.Boolean", MessageId="2#")];

bool SvnClient::GetRevisionPropertyList(SvnUriTarget^ target, [Out] IDictionary<String^, Object^>^% list)
{
	if(!target)
		throw gcnew ArgumentNullException("target");

	return GetRevisionPropertyList(target, gcnew SvnRevisionPropertyListArgs(), list);
}

bool SvnClient::GetRevisionPropertyList(SvnUriTarget^ target, SvnRevisionPropertyListArgs^ args, [Out] IDictionary<String^, Object^>^% list)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	// We allow a null listHandler; the args object might just handle it itself

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	apr_hash_t *props = nullptr;
	svn_revnum_t set_rev = 0;

	svn_error_t* r = svn_client_revprop_list(
		&props,
		pool.AllocString(target->ToString()),
		target->Revision->AllocSvnRevision(%pool),
		&set_rev,			
		CtxHandle,
		pool.Handle);

	if(!r && props)
	{
		list = CreatePropertyDictionary(props, %pool);
	}

	return args->HandleResult(this, r);
}
