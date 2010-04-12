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

	if (_handlers == nullptr)
		_handlers = gcnew Stack<SvnFsOperationRetryOverride^>();

	_onStack = _handlers;
	_onStack->Push(this);
}

SvnFsOperationRetryOverride::~SvnFsOperationRetryOverride()
{
	// Release on the thread where we were allocated
	if (_onStack != nullptr)
	{
		if (_onStack->Count > 0)
			_onStack->Pop();
		_onStack = nullptr;
	}
}

bool SvnFsOperationRetryOverride::OnRetryLoopInvocation(int nr, int error, int osError, const char *expr)
{
	if (nr >= _maxRetries)
	{
		if (_intOnly)
			return false;
		else
			return OnRetryLoopInvocation(nr, (SvnAprErrorCode)error, (SvnWindowsErrorCode)osError, gcnew String(expr));
	}

	return true;
}

bool SvnFsOperationRetryOverride::OnRetryLoopInvocation(int nr, SvnAprErrorCode aprError, SvnWindowsErrorCode windowsError, String ^expr)
{
	UNUSED_ALWAYS(nr);
	UNUSED_ALWAYS(aprError);
	UNUSED_ALWAYS(windowsError);
	UNUSED_ALWAYS(expr);
	return false;
}

int SvnFsOperationRetryOverride::RetryLoopHandler(int n, int err, int os_err, const char *expr)
{
	// Look at the per-thread handlers
	if (_handlers && _handlers->Count > 0)
	{
		SvnFsOperationRetryOverride^ p = _handlers->Peek();

		if (p)
			return p->OnRetryLoopInvocation(n, err, os_err, expr);
	}

	return true;
}
