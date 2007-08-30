#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::Switch(String^ path, SvnUriTarget^ target)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!target)
		throw gcnew ArgumentNullException("target");

	__int64 revision;

	Switch(path, target, gcnew SvnSwitchArgs(), revision);
}

void SvnClient::Switch(String^ path, SvnUriTarget^ target, [Out] __int64% revision)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!target)
		throw gcnew ArgumentNullException("target");

	Switch(path, target, gcnew SvnSwitchArgs(), revision);
}


bool SvnClient::Switch(String^ path, SvnUriTarget^ target, SvnSwitchArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!target)
		throw gcnew ArgumentNullException("target");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	__int64 revision;

	return Switch(path, target, args, revision);
}

bool SvnClient::Switch(String^ path, SvnUriTarget^ target, SvnSwitchArgs^ args, [Out] __int64% revision)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!target)
		throw gcnew ArgumentNullException("args");

	switch(target->Revision->Type)
	{
	case SvnRevisionType::Number:
	case SvnRevisionType::Head:
	case SvnRevisionType::Date:
		break;
	default:
		// Throw the error before we allocate the unmanaged resources
		throw gcnew ArgumentException(SharpSvnStrings::RevisionTypeMustBeHeadDateOrSpecific, "target");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_revnum_t rev = 0;
	svn_opt_revision_t toRev = target->Revision->ToSvnRevision();

	svn_error_t *r = svn_client_switch2(
		&rev,
		pool.AllocPath(path),
		pool.AllocString(target->TargetName),
		&toRev,
		(svn_depth_t)args->Depth,
		args->IgnoreExternals,
		args->AllowUnversionedObstructions,
		CtxHandle,
		pool.Handle);

	revision = rev;

	return args->HandleResult(r);
}
