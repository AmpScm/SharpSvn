#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;

void SvnClient::Add(String^ path)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	Add(path, gcnew SvnAddArgs());
}

void SvnClient::Add(String^ path, SvnDepth depth)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	SvnAddArgs ^args = gcnew SvnAddArgs();

	args->Depth = depth;

	Add(path, args);
}

bool SvnClient::Add(String^ path, SvnAddArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	EnsureState(SvnContextState::ConfigLoaded);

	if(_currentArgs)
		throw gcnew InvalidOperationException(SharpSvnStrings::SvnClientOperationInProgress);

	AprPool pool(%_pool);
	_currentArgs = args;
	try
	{
		svn_error_t *r = svn_client_add4(
			pool.AllocPath(path),
			IsRecursive(args->Depth),
			args->Force,
			args->NoIgnore,
			args->AddParents,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}
