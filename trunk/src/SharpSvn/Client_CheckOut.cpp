#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;

void SvnClient::CheckOut(SvnUriTarget^ url, String^ path)
{
	if(!url)
		throw gcnew ArgumentNullException("url");
	else if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	__int64 revision;
	CheckOut(url, path, gcnew SvnCheckOutArgs(), revision);
}

void SvnClient::CheckOut(SvnUriTarget^ url, String^ path, [Out] __int64% revision)
{
	if(!url)
		throw gcnew ArgumentNullException("url");
	else if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	CheckOut(url, path, gcnew SvnCheckOutArgs(), revision);
}

bool SvnClient::CheckOut(SvnUriTarget^ url, String^ path, SvnCheckOutArgs^ args)
{
	if(!url)
		throw gcnew ArgumentNullException("url");
	else if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	__int64 revision;

	return CheckOut(url, path, args, revision);
}

bool SvnClient::CheckOut(SvnUriTarget^ url, String^ path, SvnCheckOutArgs^ args, [Out] __int64% revision)
{
	if(!url)
		throw gcnew ArgumentNullException("url");
	else if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else
		switch(args->Revision->Type)
	{
		case SvnRevisionType::Date:
		case SvnRevisionType::Number:
		case SvnRevisionType::Head:
			break;
		default:
			// Throw the error before we allocate the unmanaged resources
			throw gcnew ArgumentException(SharpSvnStrings::RevisionTypeMustBeHeadDateOrSpecific , "args");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	svn_revnum_t version = 0;

	//svn_opt_peg_Re
	svn_opt_revision_t pegRev = url->Revision->ToSvnRevision();
	svn_opt_revision_t coRev = args->Revision->ToSvnRevision();

	svn_error_t* err = svn_client_checkout3(&version,
		pool.AllocString(url->TargetName),
		pool.AllocPath(path),
		&pegRev,
		&coRev,
		(svn_depth_t)args->Depth,
		args->IgnoreExternals,
		args->AllowUnversionedObstructions,
		CtxHandle,
		pool.Handle);

	revision = version;

	return args->HandleResult(err);
}