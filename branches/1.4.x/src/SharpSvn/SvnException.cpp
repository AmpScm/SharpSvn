#include "stdafx.h"

#include "SvnAll.h"
#include <svn_config.h>

using namespace SharpSvn;
using namespace SharpSvn::Apr;


SvnException^ SvnException::Create(svn_error_t *error)
{
	if(!error)
		return nullptr;

	try
	{
		if(error->apr_err == SVN_ERR_CANCELLED)
			return SvnOperationCanceledException::Create(error);
		else if(APR_STATUS_IS_EACCES(error->apr_err))
			return SvnAuthorizationException::Create(error);
		else
			return gcnew SvnException(error);
	}
	finally
	{
		svn_error_clear(error);
	}
}