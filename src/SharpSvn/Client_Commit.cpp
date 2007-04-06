#include "stdafx.h"
#include "SvnAll.h"
#include "SvnCommitArgs.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;

void SvnClient::Commit(String^ path)
{
	if(!path)
		throw gcnew ArgumentNullException("path");

	array<String^>^ paths = gcnew array<String^>(1);
	SvnCommitArgs^ args = gcnew SvnCommitArgs();
	paths[0] = path;

	SvnCommitInfo^ commitInfo = nullptr;

	CommitInternal(safe_cast<ICollection<String^>^>(paths), args, false, commitInfo);
}

void SvnClient::Commit(String^ path, [Out] SvnCommitInfo^% commitInfo)
{
	if(!path)
		throw gcnew ArgumentNullException("path");

	array<String^>^ paths = gcnew array<String^>(1);
	SvnCommitArgs^ args = gcnew SvnCommitArgs();
	paths[0] = path;

	CommitInternal(safe_cast<ICollection<String^>^>(paths), args, true, commitInfo);
}

void SvnClient::Commit(ICollection<String^>^ paths)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");

	SvnCommitArgs^ args = gcnew SvnCommitArgs();

	SvnCommitInfo^ commitInfo = nullptr;

	CommitInternal(paths, args, false, commitInfo);
}

void SvnClient::Commit(ICollection<String^>^ paths, [Out] SvnCommitInfo^% commitInfo)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");

	SvnCommitArgs^ args = gcnew SvnCommitArgs();

	CommitInternal(paths, args, true, commitInfo);
}

bool SvnClient::Commit(String^ path, SvnCommitArgs^ args)
{
	if(!path)
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	array<String^>^ paths = gcnew array<String^>(1);
	paths[0] = path;

	SvnCommitInfo^ commitInfo = nullptr;

	return CommitInternal(safe_cast<ICollection<String^>^>(paths), args, false, commitInfo);
}

bool SvnClient::Commit(String^ path, SvnCommitArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if(!path)
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	array<String^>^ paths = gcnew array<String^>(1);
	paths[0] = path;

	return CommitInternal(safe_cast<ICollection<String^>^>(paths), args, true, commitInfo);
}

bool SvnClient::Commit(ICollection<String^>^ paths, SvnCommitArgs^ args)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitInfo^ commitInfo = nullptr;

	return CommitInternal(paths, args, false, commitInfo);
}

bool SvnClient::Commit(ICollection<String^>^ paths, SvnCommitArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return CommitInternal(paths, args, true, commitInfo);
}

bool SvnClient::CommitInternal(ICollection<String^>^ paths, SvnCommitArgs^ args, bool requireInfo, [Out] SvnCommitInfo^% commitInfo)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!args)
		throw gcnew ArgumentNullException("args");	
	else if(!_pool)
		throw gcnew ObjectDisposedException("SvnClient");

	for each(String^ path in paths)
	{
		if(String::IsNullOrEmpty(path))
			throw gcnew ArgumentException("member of paths is null", "paths");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a SvnClient instance can handle only one command at a time");

	AprPool pool(_pool);
	_currentArgs = args;
	try
	{
		AprArray<String^, AprCStrPathMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCStrPathMarshaller^>(paths, %pool);

		svn_commit_info_t *commitInfoPtr = nullptr;

		svn_error_t *r = svn_client_commit4(
			&commitInfoPtr,
			aprPaths->Handle,
			(svn_depth_t)args->Depth,
			args->KeepLocks,
			args->KeepChangelist,
			args->Changelist ? pool.AllocString(args->Changelist) : nullptr,
			CtxHandle,
			pool.Handle);

		if(requireInfo && commitInfoPtr)
			commitInfo = gcnew SvnCommitInfo(commitInfoPtr);
		else
			commitInfo = nullptr;

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}