#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::CleanUp(String ^path)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	CleanUp(path, gcnew SvnCleanUpArgs());
}

bool SvnClient::CleanUp(String ^path, SvnCleanUpArgs^ args)
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
		svn_error_t *r = svn_client_cleanup(
			pool.AllocPath(path),
			CtxHandle,
			pool.Handle);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}
