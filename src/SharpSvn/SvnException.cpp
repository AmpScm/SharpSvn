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
		switch(error->apr_err)
		{
		case SVN_ERR_CANCELLED:
			return gcnew SvnOperationCanceledException(error);

		case SVN_ERR_CEASE_INVOCATION:
			return gcnew SvnOperationCompletedException(error);

		default:
			if(APR_STATUS_IS_EACCES(error->apr_err))
				return gcnew SvnAuthorizationException(error);
			else
				return gcnew SvnException(error);
		}
	}
	finally
	{
		svn_error_clear(error);
	}
}