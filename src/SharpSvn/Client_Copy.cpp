#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::Copy(SvnTarget^ sourceTarget, String^ toPath)
{
	if(!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if(String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");

	Copy(sourceTarget, toPath, gcnew SvnCopyArgs());
}

bool SvnClient::Copy(SvnTarget^ sourceTarget, String^ toPath, SvnCopyArgs^ args)
{
	if(!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if(String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!IsNotUri(toPath))
		throw gcnew ArgumentException("Path looks like a Uri", "toPath");
	else if(!_pool)
		throw gcnew ObjectDisposedException("SvnClient");

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(_pool);
	_currentArgs = args;
	try
	{
		svn_commit_info_t* pInfo = nullptr;
		svn_opt_revision_t rev = sourceTarget->Revision->ToSvnRevision();

		svn_error_t *r = svn_client_copy3(
			&pInfo,
			pool.AllocString(sourceTarget->TargetName),
			&rev,
			pool.AllocPath(toPath),
			CtxHandle,
			pool.Handle);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}

void SvnClient::RemoteCopy(SvnUriTarget^ sourceTarget, Uri^ toUri)
{
	if(!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");

	SvnCommitInfo^ commitInfo = nullptr;

	RemoteCopy(sourceTarget, toUri, gcnew SvnCopyArgs(), commitInfo);
}

void SvnClient::RemoteCopy(SvnUriTarget^ sourceTarget, Uri^ toUri, [Out] SvnCommitInfo^% commitInfo)
{
	if(!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");

	RemoteCopy(sourceTarget, toUri, gcnew SvnCopyArgs(), commitInfo);
}

bool SvnClient::RemoteCopy(SvnUriTarget^ sourceTarget, Uri^ toUri, SvnCopyArgs^ args)
{
	if(!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitInfo^ commitInfo = nullptr;
	return RemoteCopy(sourceTarget, toUri, args, commitInfo);
}

bool SvnClient::RemoteCopy(SvnUriTarget^ sourceTarget, Uri^ toUri, SvnCopyArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if(!sourceTarget)
		throw gcnew ArgumentNullException("sourceTarget");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!_pool)
		throw gcnew ObjectDisposedException("SvnClient");

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(_pool);
	_currentArgs = args;
	try
	{
		svn_commit_info_t* pInfo = nullptr;
		svn_opt_revision_t rev = sourceTarget->Revision->ToSvnRevision();

		svn_error_t *r = svn_client_copy3(
			&pInfo,
			pool.AllocString(sourceTarget->TargetName),
			&rev,
			pool.AllocString(toUri->ToString()),
			CtxHandle,
			pool.Handle);

		if(pInfo)
			commitInfo = gcnew SvnCommitInfo(pInfo);
		else
			commitInfo = nullptr;

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}
