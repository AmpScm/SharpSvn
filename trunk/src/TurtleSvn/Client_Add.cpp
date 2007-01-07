#include "stdafx.h"
#include "SvnAll.h"

using namespace TurtleSvn::Apr;
using namespace TurtleSvn;

void SvnClient::Add(String^ path)
{
	if(!path)
		throw gcnew ArgumentNullException("path");

	Add(path, gcnew SvnAddArgs());
}

void SvnClient::Add(String^ path, bool notRecursive)
{
	if(!path)
		throw gcnew ArgumentNullException("path");

	SvnAddArgs ^args = gcnew SvnAddArgs();

	args->NotRecursive = notRecursive;

	Add(path, args);
}

bool SvnClient::Add(String^ path, SvnAddArgs^ args)
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
		svn_error_t *r = svn_client_add3(
			pool->AllocString(path),
			!args->NotRecursive,
			args->Force,
			args->NoIgnore,
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
