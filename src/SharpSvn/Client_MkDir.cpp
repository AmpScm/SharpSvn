#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;


void SvnClient::MkDir(String^ path)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	MkDir(path, gcnew SvnMkDirArgs());
}

bool SvnClient::MkDir(String^ path, SvnMkDirArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	array<String^>^ paths = gcnew array<String^>(1);
	paths[1] = path;

	return MkDir(safe_cast<ICollection<String^>^>(paths), args);
}


void SvnClient::MkDir(ICollection<String^>^ paths)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");

	MkDir(paths, gcnew SvnMkDirArgs());
}

bool SvnClient::MkDir(ICollection<String^>^ paths, SvnMkDirArgs^ args)
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

		svn_error_t *r = svn_client_mkdir3(
			&commitInfo,			
			aprPaths->Handle,
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

void SvnClient::RemoteMkDir(Uri^ uri)
{
	if(!uri)
		throw gcnew ArgumentNullException("uri");

	RemoteMkDir(uri, gcnew SvnMkDirArgs());
}

bool SvnClient::RemoteMkDir(Uri^ uri, SvnMkDirArgs^ args)
{
	if(!uri)
		throw gcnew ArgumentNullException("uri");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitInfo^ commitInfo;

	return RemoteMkDir(uri, args, commitInfo);
}

bool SvnClient::RemoteMkDir(Uri^ uri, SvnMkDirArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if(!uri)
		throw gcnew ArgumentNullException("uri");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	array<Uri^>^ uris = gcnew array<Uri^>(1);
	uris[1] = uri;

	return RemoteMkDir(safe_cast<ICollection<Uri^>^>(uris), args, commitInfo);
}

bool SvnClient::RemoteMkDir(ICollection<Uri^>^ uris, SvnMkDirArgs^ args)
{
	if(!uris)
		throw gcnew ArgumentNullException("uris");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitInfo^ commitInfo;

	return RemoteMkDir(uris, args, commitInfo);

}

bool SvnClient::RemoteMkDir(ICollection<Uri^>^ uris, SvnMkDirArgs^ args, [Out] SvnCommitInfo^% commitInfo)
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

		svn_error_t *r = svn_client_mkdir3(
			&commit_info,
			aprPaths->Handle,
			args->MakeParents,
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
