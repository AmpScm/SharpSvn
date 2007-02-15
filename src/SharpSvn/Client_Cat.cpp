#include "stdafx.h"
#include "SvnAll.h"
#include "SvnStreamWrapper.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;


void SvnClient::Cat(SvnTarget^ target, Stream^ toStream)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!toStream)
		throw gcnew ArgumentNullException("toStream");

	Cat(target, toStream, gcnew SvnCatArgs());
}

bool SvnClient::Cat(SvnTarget^ target, Stream^ toStream, SvnCatArgs^ args)
{
	if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!toStream)
		throw gcnew ArgumentNullException("toStream");
	else if(!args)
		throw gcnew ObjectDisposedException("args");
	
	EnsureState(SvnContextState::ConfigLoaded);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(_pool);
	SvnStreamWrapper wrapper(toStream, false, true, %pool);
	_currentArgs = args;
	try
	{
		svn_opt_revision_t pegRev = target->Revision->ToSvnRevision();
		svn_opt_revision_t rev = args->Revision->ToSvnRevision();

		svn_error_t *r = svn_client_cat2(
			wrapper.Handle,
			pool.AllocString(target->TargetName),
			&pegRev,
			&rev,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}

}