#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::Move(String^ sourcePath, String^ toPath)
{
	if(String::IsNullOrEmpty(sourcePath))
		throw gcnew ArgumentNullException("sourcePath");
	else if(String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");

	Move(sourcePath, toPath, gcnew SvnMoveArgs());
}

bool SvnClient::Move(String^ sourcePath, String^ toPath, SvnMoveArgs^ args)
{
	if(String::IsNullOrEmpty(sourcePath))
		throw gcnew ArgumentNullException("sourcePath");
	else if(String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!IsNotUri(sourcePath))
		throw gcnew ArgumentException("Path looks like a Uri", "sourcePath");
	else if(!IsNotUri(toPath))
		throw gcnew ArgumentException("Path looks like a Uri", "toPath");
	else if(!_pool)
		throw gcnew ObjectDisposedException("SvnClient");

	EnsureState(SvnContextState::ConfigLoaded);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(_pool);
	_currentArgs = args;
	try
	{
		svn_commit_info_t* pInfo = nullptr;

		svn_error_t *r = svn_client_move4(
			&pInfo,
			pool.AllocPath(sourcePath),
			pool.AllocPath(toPath),
			args->Force,
			//args->MoveAsChild,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}

void SvnClient::RemoteMove(Uri^ sourceUri, Uri^ toUri)
{
	if(!sourceUri)
		throw gcnew ArgumentNullException("sourceUri");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");

	SvnCommitInfo^ commitInfo = nullptr;

	RemoteMove(sourceUri, toUri, gcnew SvnMoveArgs(), commitInfo);
}

void SvnClient::RemoteMove(Uri^ sourceUri, Uri^ toUri, [Out] SvnCommitInfo^% commitInfo)
{
	if(!sourceUri)
		throw gcnew ArgumentNullException("sourceUri");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");

	RemoteMove(sourceUri, toUri, gcnew SvnMoveArgs(), commitInfo);
}

bool SvnClient::RemoteMove(Uri^ sourceUri, Uri^ toUri, SvnMoveArgs^ args)
{
	if(!sourceUri)
		throw gcnew ArgumentNullException("sourceUri");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitInfo^ commitInfo = nullptr;
	return RemoteMove(sourceUri, toUri, args, commitInfo);
}

bool SvnClient::RemoteMove(Uri^ sourceUri, Uri^ toUri, SvnMoveArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if(!sourceUri)
		throw gcnew ArgumentNullException("sourceUri");
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

		svn_error_t *r = svn_client_move4(
			&pInfo,
			pool.AllocString(sourceUri->ToString()),
			pool.AllocString(toUri->ToString()),
			args->Force,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}
