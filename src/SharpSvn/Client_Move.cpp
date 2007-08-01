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

	Move(NewSingleItemCollection(sourcePath), toPath, gcnew SvnMoveArgs());
}

void SvnClient::Move(ICollection<String^>^ sourcePaths, String^ toPath)
{
	if(!sourcePaths)
		throw gcnew ArgumentNullException("sourcePaths");
	else if(String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");

	Move(sourcePaths, toPath, gcnew SvnMoveArgs());
}

bool SvnClient::Move(String^ sourcePath, String^ toPath, SvnMoveArgs^ args)
{
	if(String::IsNullOrEmpty(sourcePath))
		throw gcnew ArgumentNullException("sourcePath");
	else if(String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return Move(NewSingleItemCollection(sourcePath), toPath, args);
}

bool SvnClient::Move(ICollection<String^>^ sourcePaths, String^ toPath, SvnMoveArgs^ args)
{
	if(!sourcePaths)
		throw gcnew ArgumentNullException("sourcePaths");
	else if(String::IsNullOrEmpty(toPath))
		throw gcnew ArgumentNullException("toPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");	
	else if(!_pool)
		throw gcnew ObjectDisposedException("SvnClient");

	for each(String^ s in sourcePaths)
	{
		if(String::IsNullOrEmpty(s))
			throw gcnew ArgumentException("Path is empty", "sourcePaths");
		else if(!IsNotUri(s))
			throw gcnew ArgumentException("Path looks like a Uri", "sourcePaths");
	}

	EnsureState(SvnContextState::ConfigLoaded);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(_pool);
	_currentArgs = args;
	try
	{
		svn_commit_info_t* pInfo = nullptr;

		svn_error_t *r = svn_client_move5(
			&pInfo,
			AllocPathArray(sourcePaths, %pool),
			pool.AllocPath(toPath),
			args->Force,
			args->AlwaysMoveAsChild || (sourcePaths->Count > 1),
			args->MakeParents,
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

	RemoteMove(NewSingleItemCollection(sourceUri), toUri, gcnew SvnMoveArgs(), commitInfo);
}

void SvnClient::RemoteMove(ICollection<Uri^>^ sourceUris, Uri^ toUri)
{
	if(!sourceUris)
		throw gcnew ArgumentNullException("sourceUris");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");

	SvnCommitInfo^ commitInfo = nullptr;

	RemoteMove(sourceUris, toUri, gcnew SvnMoveArgs(), commitInfo);
}

void SvnClient::RemoteMove(Uri^ sourceUri, Uri^ toUri, [Out] SvnCommitInfo^% commitInfo)
{
	if(!sourceUri)
		throw gcnew ArgumentNullException("sourceUri");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");

	RemoteMove(NewSingleItemCollection(sourceUri), toUri, gcnew SvnMoveArgs(), commitInfo);
}

void SvnClient::RemoteMove(ICollection<Uri^>^ sourceUris, Uri^ toUri, [Out] SvnCommitInfo^% commitInfo)
{
	if(!sourceUris)
		throw gcnew ArgumentNullException("sourceUris");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");

	RemoteMove(sourceUris, toUri, gcnew SvnMoveArgs(), commitInfo);
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
	return RemoteMove(NewSingleItemCollection(sourceUri), toUri, args, commitInfo);
}

bool SvnClient::RemoteMove(ICollection<Uri^>^ sourceUris, Uri^ toUri, SvnMoveArgs^ args)
{
	if(!sourceUris)
		throw gcnew ArgumentNullException("sourceUris");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitInfo^ commitInfo = nullptr;
	return RemoteMove(sourceUris, toUri, args, commitInfo);
}

bool SvnClient::RemoteMove(Uri^ sourceUri, Uri^ toUri, SvnMoveArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if(!sourceUri)
		throw gcnew ArgumentNullException("sourceUri");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return RemoteMove(NewSingleItemCollection(sourceUri), toUri, args, commitInfo);
}

bool SvnClient::RemoteMove(ICollection<Uri^>^ sourceUris, Uri^ toUri, SvnMoveArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if(!sourceUris)
		throw gcnew ArgumentNullException("sourceUris");
	else if(!toUri)
		throw gcnew ArgumentNullException("toUri");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!_pool)
		throw gcnew ObjectDisposedException("SvnClient");

	System::Collections::Generic::List<String^>^ uris = gcnew System::Collections::Generic::List<String^>(sourceUris->Count);

	for each(Uri^ u in sourceUris)
	{
		uris->Add(u->ToString());
	}

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(_pool);
	_currentArgs = args;
	try
	{
		svn_commit_info_t* commitInfoPtr = nullptr;

		svn_error_t *r = svn_client_move5(
			&commitInfoPtr,
			AllocArray(uris, %pool),
			pool.AllocString(toUri->ToString()),
			args->Force,
			args->AlwaysMoveAsChild || (sourceUris->Count > 1),
			args->MakeParents,
			CtxHandle,
			pool.Handle);

		commitInfo = commitInfoPtr ? gcnew SvnCommitInfo(commitInfoPtr) : nullptr;

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}
