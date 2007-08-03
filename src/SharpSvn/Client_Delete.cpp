#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;


void SvnClient::Delete(String^ path)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	Delete(path, gcnew SvnDeleteArgs());
}

bool SvnClient::Delete(String^ path, SvnDeleteArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!IsNotUri(path))
		throw gcnew ArgumentException("Path is a url; please use RemoteDelete", "path");
	else if(!_pool)
		throw gcnew ObjectDisposedException("SvnClient");

	array<String^>^ paths = gcnew array<String^>(1);
	paths[0] = path;

	return Delete(safe_cast<ICollection<String^>^>(paths), args);
}


void SvnClient::Delete(ICollection<String^>^ paths)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");

	Delete(paths, gcnew SvnDeleteArgs());
}

bool SvnClient::Delete(ICollection<String^>^ paths, SvnDeleteArgs^ args)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	for each(String^ path in paths)
	{
		if(String::IsNullOrEmpty(path))
			throw gcnew ArgumentException("member of paths is null", "paths");
		else if(!IsNotUri(path))
			throw gcnew ArgumentException("Path is a url; please use RemoteDelete", "paths");
	}

	EnsureState(SvnContextState::ConfigLoaded);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(_pool);
	_currentArgs = args;
	try
	{
		AprArray<String^, AprCStrPathMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCStrPathMarshaller^>(paths, %pool);
		svn_commit_info_t* commitInfo = nullptr;

		svn_error_t *r = svn_client_delete3(
			&commitInfo,
			aprPaths->Handle,
			args->Force,
			args->KeepLocal,
			CtxHandle,
			pool.Handle);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}


bool SvnClient::RemoteDelete(Uri^ uri, SvnDeleteArgs^ args)
{
	if(!uri)
		throw gcnew ArgumentNullException("uri");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return RemoteDelete(NewSingleItemCollection(uri), args);
}

bool SvnClient::RemoteDelete(Uri^ uri, SvnDeleteArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if(!uri)
		throw gcnew ArgumentNullException("uri");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return RemoteDelete(NewSingleItemCollection(uri), args, commitInfo);
}


bool SvnClient::RemoteDelete(ICollection<Uri^>^ uris, SvnDeleteArgs^ args)
{
	SvnCommitInfo^ commitInfo;

	return RemoteDelete(uris, args, commitInfo);
}

bool SvnClient::RemoteDelete(ICollection<Uri^>^ uris, SvnDeleteArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if(!uris)
		throw gcnew ArgumentNullException("uris");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	commitInfo = nullptr;

	array<String^>^ uriData = gcnew array<String^>(uris->Count);
	int i = 0;

	for each(Uri^ uri in uris)
	{
		if(uri == nullptr)
			throw gcnew ArgumentException("Uris contains null Uri", "uris");
		uriData[i++] = uri->ToString();
	}

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	AprPool pool(_pool);
	_currentArgs = args;
	try
	{
		AprArray<String^, AprCStrMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCStrMarshaller^>(safe_cast<ICollection<String^>^>(uriData), %pool);
		svn_commit_info_t* commit_info = nullptr;

		svn_error_t *r = svn_client_delete3(
			&commit_info,
			aprPaths->Handle,
			args->Force,
			args->KeepLocal,
			CtxHandle,
			pool.Handle);

		if(commit_info)
			commitInfo = gcnew SvnCommitInfo(commit_info);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}
