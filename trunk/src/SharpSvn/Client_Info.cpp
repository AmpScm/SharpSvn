#include "stdafx.h"
#include "SvnAll.h"

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;

void SvnClient::Info(SvnTarget^ target, EventHandler<SvnInfoEventArgs^>^ infoHandler)
{
	throw gcnew NotImplementedException();
}

bool SvnClient::Info(SvnTarget^ target, EventHandler<SvnInfoEventArgs^>^ infoHandler, SvnInfoArgs^ args)
{
	throw gcnew NotImplementedException();
}

void SvnClient::GetInfo(SvnTarget^ target, [Out] SvnInfoEventArgs^% status)
{
	throw gcnew NotImplementedException();
}

bool SvnClient::GetInfo(SvnTarget^ target, SvnInfoArgs^ args, [Out] IList<SvnInfoEventArgs^>^ statuses)
{
	throw gcnew NotImplementedException();
}