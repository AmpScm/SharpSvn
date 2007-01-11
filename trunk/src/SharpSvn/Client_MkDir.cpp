#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;


void SvnClient::MkDir(String^ path)
{
	throw gcnew NotImplementedException();
}

bool SvnClient::MkDir(String^ path, SvnMkDirArgs^ args)
{
	throw gcnew NotImplementedException();
}


void SvnClient::MkDir(ICollection<String^>^ paths)
{
	throw gcnew NotImplementedException();
}

bool SvnClient::MkDir(ICollection<String^>^ paths, SvnMkDirArgs^ args)
{
	throw gcnew NotImplementedException();
}

bool SvnClient::RemoteMkDir(Uri^ uri, SvnMkDirArgs^ args)
{
	throw gcnew NotImplementedException();
}

bool SvnClient::RemoteMkDir(Uri^ uri, SvnMkDirArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	throw gcnew NotImplementedException();
}

bool SvnClient::RemoteMkDir(ICollection<Uri^>^ uris, SvnMkDirArgs^ args)
{
	throw gcnew NotImplementedException();
}

bool SvnClient::RemoteMkDir(ICollection<Uri^>^ uris, SvnMkDirArgs^ args, [Out] SvnCommitInfo^% commitInfo)
{
	throw gcnew NotImplementedException();
}
