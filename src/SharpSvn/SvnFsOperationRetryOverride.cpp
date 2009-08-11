#include "stdafx.h"

#include "SvnAll.h"
#include "SvnFsOperationRetryOverride.h"
#include <libintl.h>
using namespace SharpSvn;
using namespace SharpSvn::Implementation;

static int __cdecl static_RetryLoopHandler(int n, int err, int os_err, const char *expr)
{
	return SvnFsOperationRetryOverride::RetryLoopHandler(n, err, os_err, expr);
}

static SvnFsOperationRetryOverride::SvnFsOperationRetryOverride()
{
	do_sharpsvn_retry = static_RetryLoopHandler;
}

SvnFsOperationRetryOverride::SvnFsOperationRetryOverride(int maxRetries)
{
	if (maxRetries < 0 || maxRetries > 100)
		throw gcnew ArgumentOutOfRangeException("maxRetries");

	_maxRetries = maxRetries;
	_intOnly = GetType() == SvnFsOperationRetryOverride::typeid;
	_oldHandler = _handler;
	_handler = this;
}

SvnFsOperationRetryOverride::~SvnFsOperationRetryOverride()
{
	if (_handler == this)
	{
		_handler = _oldHandler;
		_oldHandler = nullptr;
	}
}

bool SvnFsOperationRetryOverride::OnRetryLoopInvocation(int nr, int error, int osError, const char *expr)
{
	if (nr >= _maxRetries)
	{
		if (_intOnly)
			return false;
		else
			return OnRetryLoopInvocation(nr, (SvnErrorCode)error, osError, gcnew String(expr));
	}
	
	return true;
}

bool SvnFsOperationRetryOverride::OnRetryLoopInvocation(int nr, SvnErrorCode error, int osError, String ^expr)
{
	UNUSED_ALWAYS(nr);
	UNUSED_ALWAYS(error);
	UNUSED_ALWAYS(osError);
	UNUSED_ALWAYS(expr);
	return false;
}

int SvnFsOperationRetryOverride::RetryLoopHandler(int n, int err, int os_err, const char *expr)
{
	if (_handler)
		return _handler->OnRetryLoopInvocation(n, err, os_err, expr);

	return true;
}