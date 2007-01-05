#include "stdafx.h"
#include "SvnAll.h"

using namespace TurtleSvn::Apr;
using namespace TurtleSvn;
using namespace System::Collections::Generic;

const char* SvnClient::GetEolPtr(SvnEolStyle style)
{
	switch(style)
	{
	case SvnEolStyle::Native:
		return nullptr;
	case SvnEolStyle::CrLf:
		return "CRLF";
	case SvnEolStyle::Lf:
		return "LF";
	case SvnEolStyle::Cr:
		return "CR";
	default:
		throw gcnew ArgumentOutOfRangeException("style");
	}
}


void SvnClient::Export(SvnTarget^ from, String^ toPath)
{
	if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!toPath)
		throw gcnew ArgumentNullException("toPath");

	__int64 revision;

	Export(from, toPath, gcnew SvnExportArgs(), revision);
}

void SvnClient::Export(SvnTarget^ from, String^ toPath, [Out] __int64% revision)
{
	if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!toPath)
		throw gcnew ArgumentNullException("toPath");

	Export(from, toPath, gcnew SvnExportArgs(), revision);
}

void SvnClient::Export(SvnTarget^ from, String^ toPath, bool overwrite)
{
	if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!toPath)
		throw gcnew ArgumentNullException("toPath");

	SvnExportArgs^ args = gcnew SvnExportArgs();

	args->Overwrite = overwrite;

	__int64 revision;
	Export(from, toPath, args, revision);
}

void SvnClient::Export(SvnTarget^ from, String^ toPath, bool overwrite, [Out] __int64% revision)
{
	if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!toPath)
		throw gcnew ArgumentNullException("toPath");

	SvnExportArgs^ args = gcnew SvnExportArgs();

	args->Overwrite = overwrite;

	Export(from, toPath, args, revision);
}

bool SvnClient::Export(SvnTarget^ from, String^ toPath, SvnExportArgs^ args)
{
	if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!toPath)
		throw gcnew ArgumentNullException("toPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	__int64 revision;
	Export(from, toPath, args, revision);
}

bool SvnClient::Export(SvnTarget^ from, String^ toPath, SvnExportArgs^ args, [Out] __int64% revision)
{
	if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!toPath)
		throw gcnew ArgumentNullException("toPath");
	else if(!args)
		throw gcnew ArgumentNullException("args");

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	revision = -1;

	AprPool^ pool = gcnew AprPool(_pool);
	_currentArgs = args;

	try
	{
		svn_revnum_t resultRev = 0;
		svn_opt_revision_t pegRev = from->Revision->ToSvnRevision();
		svn_opt_revision_t rev = args->Revision->ToSvnRevision();

		svn_error_t* r = svn_client_export3(
			&resultRev, 
			pool->AllocString(from->TargetName), 
			pool->AllocString(toPath), 
			&pegRev, 
			&rev, 
			args->Overwrite, 
			args->IgnoreExternals, 
			!args->NotRecursive,
			GetEolPtr(args->EolStyle),
			CtxHandle,
			pool->Handle);

		if(args->HandleResult(r))
		{
			revision = resultRev;
		}
		else
			return false;
	}
	finally
	{
		_currentArgs = nullptr;
		delete pool;
	}

}