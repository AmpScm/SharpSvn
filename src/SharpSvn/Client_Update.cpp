#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::Update(String^ path)
{
	if(!path)
		throw gcnew ArgumentNullException("path");

	array<String^>^ paths = gcnew array<String^>(1);

	paths[0] = path;

	UpdateInternal(safe_cast<ICollection<String^>^>(paths), gcnew SvnUpdateArgs(), nullptr);
}

void SvnClient::Update(String^ path, [Out] __int64% revision)
{
	if(!path)
		throw gcnew ArgumentNullException("path");

	array<__int64>^ revisions = gcnew array<__int64>(1);
	array<String^>^ paths = gcnew array<String^>(1);
	
	revision = -1;

	paths[0] = path;

	UpdateInternal(safe_cast<ICollection<String^>^>(paths), gcnew SvnUpdateArgs(), revisions);

	revision = revisions[0];
}


bool SvnClient::Update(String^ path, SvnUpdateArgs^ args)
{
	if(!path)
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	array<String^>^ paths = gcnew array<String^>(1);

	paths[0] = path;

	return UpdateInternal(safe_cast<ICollection<String^>^>(paths), args, nullptr);
}

bool SvnClient::Update(String^ path, SvnUpdateArgs^ args, [Out] __int64% revision)
{
	if(!path)
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");
	else if(!_pool)
		throw gcnew ObjectDisposedException("SvnClient");

	array<__int64>^ revisions = gcnew array<__int64>(1);
	array<String^>^ paths = gcnew array<String^>(1);

	paths[0] = path;

	revision = 0;

	try
	{
		return UpdateInternal(safe_cast<ICollection<String^>^>(paths), args, revisions);
	}
	finally
	{
		revision = revisions[0];
	}
}

void SvnClient::Update(ICollection<String^>^ paths)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");

	UpdateInternal(paths, gcnew SvnUpdateArgs(), nullptr);
}

void SvnClient::Update(ICollection<String^>^ paths, [Out] IList<__int64>^% revisions)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");

	array<__int64>^ revs = gcnew array<__int64>(paths->Count);

	try
	{
		UpdateInternal(paths, gcnew SvnUpdateArgs(), revs);
	}
	finally
	{
		revisions = safe_cast<IList<__int64>^>(revs);
	}
}


bool SvnClient::Update(ICollection<String^>^ paths, SvnUpdateArgs^ args)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	return UpdateInternal(paths, args, nullptr);
}

bool SvnClient::Update(ICollection<String^>^ paths, SvnUpdateArgs^ args, [Out] IList<__int64>^% revisions)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	array<__int64>^ revs = gcnew array<__int64>(paths->Count);
	revisions = nullptr;

	try
	{
		return UpdateInternal(paths, args, revs);
	}
	finally
	{
		revisions = safe_cast<IList<__int64>^>(revs);
	}
}

bool SvnClient::UpdateInternal(ICollection<String^>^ paths, SvnUpdateArgs^ args, array<__int64>^ revisions)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!args)
		throw gcnew ArgumentNullException("args");
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

	for each(String^ s in paths)
	{
		if(String::IsNullOrEmpty(s))
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

		apr_array_header_t* revs = nullptr;
		svn_opt_revision_t uRev = args->Revision->ToSvnRevision();

		svn_error_t *r = svn_client_update3(revisions ? &revs : nullptr,
			aprPaths->Handle,
			&uRev,
			(svn_depth_t)args->Depth,
			args->IgnoreExternals,
			args->AllowUnversionedObstructions,
			CtxHandle,
			pool.Handle);

		if(args->HandleResult(r) && revisions)
		{			
			AprArray<__int64, AprSvnRevNumMarshaller^>^ aprRevs = gcnew AprArray<__int64, AprSvnRevNumMarshaller^>(revs, %pool);

			for(int i = 0; i < aprRevs->Count; i++)
				revisions[i] = aprRevs[i];

			return true;
		}

		return false;
	}
	finally
	{
		_currentArgs = nullptr;
	}
}
