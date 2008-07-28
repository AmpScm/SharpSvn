// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#include "SvnAll.h"
#include <svn_config.h>

using namespace SharpSvn::Implementation;
using namespace SharpSvn;

bool SvnClientArgs::HandleResult(SvnClientContext^ client, svn_error_t *error)
{
	// Note: client can be null if called from a not SvnClient command
	if (!error)
	{
		_exception = nullptr;
		return true;
	}

	apr_status_t err = error->apr_err;

	_exception = SvnException::Create(error);

	if (err == SVN_ERR_CEASE_INVOCATION)
		return false;

	return HandleResult(client, _exception); // Releases error
}

bool SvnClientArgs::HandleResult(SvnClientContext^ client, SvnException^ exception)
{
	_exception = exception;

	if (!_exception)
		return true;

	if (_exception->SubversionErrorCode == SVN_ERR_CEASE_INVOCATION)
		return false;

	SvnClient^ svnClient = dynamic_cast<SvnClient^>(client);

	SvnErrorEventArgs^ ea = gcnew SvnErrorEventArgs(_exception);
	if (svnClient)
	{
		svnClient->HandleClientError(ea);
	}
	else
		OnSvnError(ea);

	if (ea->Cancel)
		return false;

	if (!ThrowOnCancel && _exception->SubversionErrorCode == SVN_ERR_CANCELLED)
		return false;
	else if (ThrowOnError)
		throw _exception;
	else
		return false;
}

void SvnClientArgs::Prepare()
{
	LastException = nullptr;
}

bool SvnClientArgs::IsLastInvocationCanceled::get()
{
	return _exception && dynamic_cast<SvnOperationCompletedException^>(_exception);
}
