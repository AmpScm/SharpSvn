#include "stdafx.h"

#include "SvnAll.h"
#include <svn_config.h>

using namespace SharpSvn::Apr;
using namespace SharpSvn;

bool SvnClientArgs::HandleResult(SvnClientContext^ client, svn_error_t *error)
{
	if(!error)
		return true;

	apr_status_t err = error->apr_err;
	_exception = SvnException::Create(error); // Releases error

	if(err == SVN_ERR_CEASE_INVOCATION)
		return false;

	SvnClient^ svnClient = dynamic_cast<SvnClient^>(client);

	if(_exception && svnClient)
	{
		SvnErrorEventArgs^ ea = gcnew SvnErrorEventArgs(_exception);

		if(svnClient)
		{
			svnClient->HandleClientError(ea);

			if(ea->Cancel)
				return false;
		}
	}

	if(ThrowOnError)
		throw _exception;
	else
		return false;
}

bool SvnClientArgs::InvokationCeased::get()
{
	return _exception && dynamic_cast<SvnOperationCompletedException^>(_exception);
}
