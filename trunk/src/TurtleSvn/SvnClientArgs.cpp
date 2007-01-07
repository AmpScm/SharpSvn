#include "stdafx.h"

#include "SvnAll.h"
#include <svn_config.h>

using namespace TurtleSvn::Apr;
using namespace TurtleSvn;

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

