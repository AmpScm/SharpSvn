#include "stdafx.h"

#include "SvnAll.h"
#include <svn_config.h>

using namespace SharpSvn::Apr;
using namespace SharpSvn;

bool SvnClientArgs::HandleResult(svn_error_t *error)
{
	if(!error)
		return true;

	_exception = SvnException::Create(error);

	if(ThrowOnError)
		throw _exception;
	else
		return false;
}

