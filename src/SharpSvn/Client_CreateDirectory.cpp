#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;


void SvnClient::CreateDirectory(String^ path)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	CreateDirectory(NewSingleItemCollection(path), gcnew SvnCreateDirectoryArgs());
}

bool SvnClient::CreateDirectory(String^ path, SvnCreateDirectoryArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return CreateDirectory(NewSingleItemCollection(path), args);
}


void SvnClient::CreateDirectory(ICollection<String^>^ paths)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");

	CreateDirectory(paths, gcnew SvnCreateDirectoryArgs());
}

bool SvnClient::CreateDirectory(ICollection<String^>^ paths, SvnCreateDirectoryArgs^ args)
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

	AprPool pool(%_pool);
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

void SvnClient::RemoteCreateDirectory(Uri^ uri)
{
	if(!uri)
		throw gcnew ArgumentNullException("uri");

	RemoteCreateDirectory(uri, gcnew SvnCreateDirectoryArgs());
}

bool SvnClient::RemoteCreateDirectory(Uri^ uri, SvnCreateDirectoryArgs^ args)
{
	if(!uri)
		throw gcnew ArgumentNullException("uri");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitInfo^ commitInfo;

	return RemoteCreateDirectory(uri, args, commitInfo);
}

bool SvnClient::RemoteCreateDirectory(Uri^ uri, SvnCreateDirectoryArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	if(!uri)
		throw gcnew ArgumentNullException("uri");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return RemoteCreateDirectory(NewSingleItemCollection(uri), args, commitInfo);
}

bool SvnClient::RemoteCreateDirectory(ICollection<Uri^>^ uris, SvnCreateDirectoryArgs^ args)
{
	if(!uris)
		throw gcnew ArgumentNullException("uris");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	SvnCommitInfo^ commitInfo;

	return RemoteCreateDirectory(uris, args, commitInfo);

}

bool SvnClient::RemoteCreateDirectory(ICollection<Uri^>^ uris, SvnCreateDirectoryArgs^ args, [Out] SvnCommitInfo^% commitInfo)
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

	AprPool pool(%_pool);
	_currentArgs = args;
	try
	{
		AprArray<String^, AprCanonicalMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCanonicalMarshaller^>(safe_cast<ICollection<String^>^>(uriData), %pool);
		svn_commit_info_t* commit_info = nullptr;

		svn_error_t *r = svn_client_mkdir3(
			&commit_info,
			aprPaths->Handle,
			args->MakeParents,
			CtxHandle,
			pool.Handle);

		if(commit_info)
			commitInfo = gcnew SvnCommitInfo(commit_info, %pool);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}

}
