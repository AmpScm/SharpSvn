#pragma once
#include "AprPool.h"
#include "AprArray.h"
#include "SvnClientContext.h"

#include "SvnClient.h"
#include "SvnRepositoryClient.h"

#include "SvnPathTarget.h"
#include "SvnUriTarget.h"

#include "SvnException.h"

#include "SvnAuthentication.h"

#define MANAGED_EXCEPTION_PREFIX "Forwarded Managed Inner Exception/SharpSvn/Handle="

svn_error_t* CreateExceptionSvnError(String^ origin, Exception^ exception);
