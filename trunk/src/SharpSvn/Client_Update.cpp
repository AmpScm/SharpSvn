#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::Update(String^ path)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	IList<__int64>^ revisions;
	Update(NewSingleItemCollection(path), gcnew SvnUpdateArgs(), revisions);
}

void SvnClient::Update(String^ path, [Out] __int64% revision)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	IList<__int64>^ revisions;
	revision = -1;

	Update(NewSingleItemCollection(path), gcnew SvnUpdateArgs(), revisions);

	revision = revisions[0];
}


bool SvnClient::Update(String^ path, SvnUpdateArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	IList<__int64>^ revisions;

	return Update(NewSingleItemCollection(path), args, revisions);
}

bool SvnClient::Update(String^ path, SvnUpdateArgs^ args, [Out] __int64% revision)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	revision = -1;

	IList<__int64>^ revisions;
	try
	{
		return Update(NewSingleItemCollection(path), args, revisions);
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

	IList<__int64>^ revisions;

	Update(paths, gcnew SvnUpdateArgs(), revisions);
}

void SvnClient::Update(ICollection<String^>^ paths, [Out] IList<__int64>^% revisions)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");

	Update(paths, gcnew SvnUpdateArgs(), revisions);
}


bool SvnClient::Update(ICollection<String^>^ paths, SvnUpdateArgs^ args)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	IList<__int64>^ revisions;

	return Update(paths, args, revisions);
}

bool SvnClient::Update(ICollection<String^>^ paths, SvnUpdateArgs^ args, [Out] IList<__int64>^% revisions)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!args)
		throw gcnew ArgumentNullException("args");
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

	revisions = nullptr;

	for each(String^ s in paths)
	{
		if(String::IsNullOrEmpty(s))
			throw gcnew ArgumentException("member of paths is null", "paths");
	}

	EnsureState(SvnContextState::AuthorizationInitialized);

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a SvnClient instance can handle only one command at a time");

	AprPool pool(%_pool);
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

			revisions = safe_cast<IList<__int64>^>(aprRevs->ToArray());

			return true;
		}

		return false;
	}
	finally
	{
		_currentArgs = nullptr;
	}
}
