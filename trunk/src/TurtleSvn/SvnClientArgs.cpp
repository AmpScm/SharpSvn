#include "stdafx.h"

#include "SvnAll.h"
#include <svn_config.h>

using namespace TurtleSvn::Apr;
using namespace TurtleSvn;

bool SvnClientArgs::HandleResult(svn_error_t *error)
{
	if(!error)
		return true;
	else if(ThrowOnError)
		throw gcnew SvnException(error);
	else
		_exception = gcnew SvnException(error);

	return false;
}