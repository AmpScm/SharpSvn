#include "stdafx.h"

#include "SvnAll.h"
#include <svn_config.h>

using namespace SharpSvn::Apr;
using namespace SharpSvn;

bool SvnClientArgs::HandleResult(svn_error_t *error)
{
	if(!error)
		return true;

	apr_status_t err = error->apr_err;
	_exception = SvnException::Create(error);

	if(ThrowOnError && err != SVN_ERR_CEASE_INVOCATION)
		throw _exception;
	else
		return false;
}

