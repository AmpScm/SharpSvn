#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;

void SvnClient::Commit(String^ path)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	SvnCommitInfo^ commitInfo = nullptr;

	Commit(NewSingleItemCollection(path), gcnew SvnCommitArgs(), commitInfo);
}

void SvnClient::Commit(String^ path, [Out] SvnCommitInfo^% commitInfo)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	Commit(NewSingleItemCollection(path), gcnew SvnCommitArgs(), commitInfo);
}

void SvnClient::Commit(ICollection<String^>^ paths)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");

	SvnCommitInfo^ commitInfo = nullptr;

	Commit(paths, gcnew SvnCommitArgs(), commitInfo);
}

void SvnClient::Commit(ICollection<String^>^ paths, [Out] SvnCommitInfo^% commitInfo)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");

	Commit(paths, gcnew SvnCommitArgs(), commitInfo);
}

bool SvnClient::Commit(String^ path, SvnCommitArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitInfo^ commitInfo = nullptr;

	return Commit(NewSingleItemCollection(path), args, commitInfo);
}

bool SvnClient::Commit(String^ path, SvnCommitArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return Commit(NewSingleItemCollection(path), args, commitInfo);
}

bool SvnClient::Commit(ICollection<String^>^ paths, SvnCommitArgs^ args)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitInfo^ commitInfo = nullptr;

	return Commit(paths, args, commitInfo);
}

bool SvnClient::Commit(ICollection<String^>^ paths, SvnCommitArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	for each(String^ path in paths)
	{
		if(String::IsNullOrEmpty(path))
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "paths");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a SvnClient instance can handle only one command at a time");

	AprPool pool(%_pool);
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

		if(commitInfoPtr)
			commitInfo = gcnew SvnCommitInfo(commitInfoPtr, %pool);
		else
			commitInfo = nullptr;

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}
