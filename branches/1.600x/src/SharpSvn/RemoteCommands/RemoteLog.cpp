#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteLogArgs.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace SharpSvn::Remote;
using namespace System::Collections::Generic;


bool SvnRemoteSession::Log(ICollection<String^>^ paths, EventHandler<SvnRemoteLogEventArgs^>^ logHandler)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");
	else if (!logHandler)
		throw gcnew ArgumentNullException("logHandler");

	return Log(paths, gcnew SvnRemoteLogArgs(), logHandler);
}

bool SvnRemoteSession::Log(ICollection<String^>^ paths, SvnRemoteLogArgs^ args, EventHandler<SvnRemoteLogEventArgs^>^ logHandler)
{
	throw gcnew NotImplementedException();
}