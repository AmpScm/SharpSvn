#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;


void SvnClient::Resolved(String^ path)
{
	if(!path)
		throw gcnew ArgumentNullException("path");

	Resolved(path, gcnew SvnResolvedArgs());
}

void SvnClient::Resolved(String^ path, SvnAccept accept)
{
	if(!path)
		throw gcnew ArgumentNullException("path");

	Resolved(path, gcnew SvnResolvedArgs(accept));
}

bool SvnClient::Resolved(String^ path, SvnResolvedArgs^ args)
{
	if(!path)
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!_pool)
		throw gcnew ObjectDisposedException("SvnClient");

	EnsureState(SvnContextState::ConfigLoaded);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(_pool);
	_currentArgs = args;
	try
	{
		svn_error_t *r = svn_client_resolved2(
			pool.AllocPath(path),
			IsRecursive(args->Depth),
			(svn_accept_t)args->Accept,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}
