#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::SetProperty(String^ path, String^ propertyName, String^ value)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!value)
		throw gcnew ArgumentNullException("value");

	SetProperty(path, propertyName, value, gcnew SvnSetPropertyArgs());
}

void SvnClient::SetProperty(String^ path, String^ propertyName, IList<char>^ bytes)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!bytes)
		throw gcnew ArgumentNullException("bytes");

	SetProperty(path, propertyName, bytes, gcnew SvnSetPropertyArgs());
}

bool SvnClient::SetProperty(String^ path, String^ propertyName, String^ value, SvnSetPropertyArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!value)
		throw gcnew ArgumentNullException("value");

	AprPool pool(_pool);

	return InternalSetProperty(path, propertyName, pool.AllocSvnString(value), args, %pool);
}

bool SvnClient::SetProperty(String^ path, String^ propertyName, IList<char>^ bytes, SvnSetPropertyArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!bytes)
		throw gcnew ArgumentNullException("bytes");

	AprPool pool(_pool);

	array<char> ^byteArray = safe_cast<array<char>>(bytes);

	if(!byteArray)
	{
		byteArray = gcnew array<char>(bytes->Count);

		bytes->CopyTo(byteArray, 0);
	}

	return InternalSetProperty(path, propertyName, pool.AllocSvnString(byteArray), args, %pool);
}

void SvnClient::DeleteProperty(String^ path, String^ propertyName)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");

	DeleteProperty(path, propertyName, gcnew SvnSetPropertyArgs());
}

bool SvnClient::DeleteProperty(String^ path, String^ propertyName, SvnSetPropertyArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!args)
		throw gcnew ArgumentNullException("args");
		
	AprPool pool(_pool);

	return InternalSetProperty(path, propertyName, nullptr, args, %pool);
}

bool SvnClient::InternalSetProperty(String^ path, String^ propertyName, const svn_string_t* value, SvnSetPropertyArgs^ args, AprPool^ pool)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(String::IsNullOrEmpty(propertyName))
		throw gcnew ArgumentNullException("propertyName");
	else if(!_pool)
		throw gcnew ObjectDisposedException("SvnClient");

	EnsureState(SvnContextState::ConfigLoaded);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	_currentArgs = args;
	try
	{
		svn_commit_info_t* pInfo = nullptr;


		svn_error_t *r = svn_client_propset3(
			&pInfo,
			pool->AllocString(propertyName),
			value,
			pool->AllocPath(path),
			IsRecursive(args->Depth),
			args->SkipChecks,
			(svn_revnum_t)args->BaseRevision,
			CtxHandle,
			pool->Handle);

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;
	}
}