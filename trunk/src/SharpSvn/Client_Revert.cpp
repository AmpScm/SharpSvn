#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;


void SvnClient::Revert(String^ path)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");

	Revert(path, gcnew SvnRevertArgs());
}

bool SvnClient::Revert(String^ path, SvnRevertArgs^ args)
{
	if(String::IsNullOrEmpty(path))
		throw gcnew ArgumentNullException("path");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	array<String^>^ paths = gcnew array<String^>(1);
	paths[0] = path;

	return Revert(safe_cast<ICollection<String^>^>(paths), args);
}

void SvnClient::Revert(ICollection<String^>^ paths)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");

	Revert(paths, gcnew SvnRevertArgs());
}

bool SvnClient::Revert(ICollection<String^>^ paths, SvnRevertArgs^ args)
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

	EnsureState(SvnContextState::ConfigLoaded);
	ArgsStore store(this, args);
	AprPool pool(%_pool);

	AprArray<String^, AprCStrPathMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCStrPathMarshaller^>(paths, %pool);

	svn_error_t *r = svn_client_revert2(
		aprPaths->Handle,
		(svn_depth_t)args->Depth,
		CtxHandle,
		pool.Handle);

	return args->HandleResult(r);
}
