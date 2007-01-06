#include "stdafx.h"

#include "SvnAll.h"
#include <svn_config.h>
#include "SvnObjBaton.h"

using namespace TurtleSvn;
using namespace TurtleSvn::Apr;


SvnException^ SvnException::Create(svn_error_t *error)
{
	if(!error)
		return nullptr;

	if(error->apr_err == SVN_ERR_CANCELLED)
		return gcnew SvnOperationCanceledException(error);
	else if(APR_STATUS_IS_EACCES(error->apr_err))
		return gcnew SvnAuthorizationException(error);
	else
		return gcnew SvnException(error);
}