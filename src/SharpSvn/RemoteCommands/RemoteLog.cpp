#include "stdafx.h"

#include "SvnAll.h"
#include "RemoteArgs/SvnRemoteSessionLog.h"

using namespace SharpSvn;
using namespace SharpSvn::Implementation;
using namespace System::Collections::Generic;


bool SvnRemoteSession::Log(ICollection<String^>^ paths, EventHandler<SvnRemoteSessionLogEventArgs^>^ logHandler)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");
	else if (!logHandler)
		throw gcnew ArgumentNullException("logHandler");

	return Log(paths, gcnew SvnRemoteSessionLogArgs(), logHandler);
}

bool SvnRemoteSession::Log(ICollection<String^>^ paths, SvnRemoteSessionLogArgs^ args, EventHandler<SvnRemoteSessionLogEventArgs^>^ logHandler)
{
	throw gcnew NotImplementedException();
}