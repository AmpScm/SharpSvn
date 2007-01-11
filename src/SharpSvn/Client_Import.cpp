#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;


void SvnClient::Import(String^ path, Uri^ target)
{
	throw gcnew NotImplementedException();
}

void SvnClient::Import(String^ path, Uri^ target, [Out] SvnCommitInfo^% commitInfo)
{
	throw gcnew NotImplementedException();
}

bool SvnClient::Import(String^ path, Uri^ target, SvnImportArgs^ args)
{
	throw gcnew NotImplementedException();
}

bool SvnClient::Import(String^ path, Uri^ target, SvnImportArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	throw gcnew NotImplementedException();
}

void SvnClient::RemoteImport(String^ path, Uri^ target)
{
	throw gcnew NotImplementedException();
}

void SvnClient::RemoteImport(String^ path, Uri^ target, [Out] SvnCommitInfo^% commitInfo)
{
	throw gcnew NotImplementedException();
}

bool SvnClient::RemoteImport(String^ path, Uri^ target, SvnImportArgs^ args)
{
	throw gcnew NotImplementedException();
}

bool SvnClient::RemoteImport(String^ path, Uri^ target, SvnImportArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	throw gcnew NotImplementedException();
}
