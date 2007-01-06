#include "stdafx.h"
#include "SvnAll.h"

using namespace TurtleSvn::Apr;
using namespace TurtleSvn;
using namespace System::Collections::Generic;

void SvnClient::CleanUp(String ^path)
{
	if(!path)
		throw gcnew ArgumentNullException("path");

	CleanUp(path, gcnew SvnCleanUpArgs());
}

bool SvnClient::CleanUp(String ^path, SvnCleanUpArgs^ args)
{
	if(!path)
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	
	EnsureState(SvnContextState::ConfigLoaded);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool^ pool = gcnew AprPool(_pool);
	_currentArgs = args;
	try
	{
		svn_error_t *r = svn_client_cleanup(
			pool->AllocString(path),
			CtxHandle,
			pool->Handle);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
		delete pool;
	}
}
