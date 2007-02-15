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
	else if(!args->Revision)
		throw gcnew ArgumentException("args.Revision is not set", "args");
	else if(!_pool)
		throw gcnew ObjectDisposedException("SvnClient");
	else
		switch(args->Revision->Type)
	{
		case SvnRevisionType::Date:
		case SvnRevisionType::Number:
		case SvnRevisionType::Head:
			break;
		default:
			// Throw the error before we allocate the unmanaged resources
			throw gcnew ArgumentException("Revision type must be head, date or number", "args");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(_pool);
	_currentArgs = args;
	try
	{
		svn_revnum_t version = 0;

		//svn_opt_peg_Re
		svn_opt_revision_t pegRev = url->Revision->ToSvnRevision();
		svn_opt_revision_t coRev = args->Revision->ToSvnRevision();

		svn_error_t* err = svn_client_checkout2(&version, 
			pool.AllocString(url->TargetName), 
			pool.AllocPath(path), 
			&pegRev, 
			&coRev, 
			!args->NotRecursive, 
			args->IgnoreExternals,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(err);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}