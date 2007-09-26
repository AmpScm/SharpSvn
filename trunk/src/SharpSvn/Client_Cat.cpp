#include "stdafx.h"
#include "SvnAll.h"
#include "SvnStreamWrapper.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

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
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	SvnStreamWrapper wrapper(toStream, false, true, %pool);

	svn_opt_revision_t pegRev = target->Revision->ToSvnRevision();
	svn_opt_revision_t rev = args->Revision->ToSvnRevision();

	svn_error_t *r = svn_client_cat2(
		wrapper.Handle,
		pool.AllocString(target->TargetName),
		&pegRev,
		&rev,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(this, r);
}