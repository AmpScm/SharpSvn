#include "stdafx.h"
#include "SvnAll.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using namespace System::Collections::Generic;
using System::IO::File;
using System::IO::FileMode;

ref class DeleteOnCloseStream : public System::IO::FileStream
{
	initonly String^ _fullPath;
public:
	DeleteOnCloseStream(String ^path, FileMode fileMode)
		: FileStream(path, fileMode)
	{
		_fullPath = path;
	}

	virtual void Close() override
	{
		FileStream::Close();
		File::Delete(_fullPath);
	}
};


void SvnClient::Diff(SvnTarget^ from, SvnTarget^ to, [Out]FileStream^% result)
{
	if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!to)
		throw gcnew ArgumentNullException("to");

	Diff(from, to, gcnew SvnDiffArgs(), result);
}

bool SvnClient::Diff(SvnTarget^ from, SvnTarget^ to, SvnDiffArgs^ args, [Out]FileStream^% result)
{
	if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!to)
		throw gcnew ArgumentNullException("to");

	EnsureState(SvnContextState::AuthorizationInitialized);

	String^ tempOut = System::IO::Path::GetTempFileName();
	String^ tempErr = System::IO::Path::GetTempFileName();

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	apr_file_t* tmpOut = nullptr;
	apr_file_t* tmpErr = nullptr;

	AprPool pool(%_pool);
	_currentArgs = args;

	try
	{
		apr_int32_t openFlags = APR_FOPEN_CREATE | APR_FOPEN_TRUNCATE | APR_BUFFERED | APR_FOPEN_WRITE;

		svn_error_t *openErr = nullptr;
		if(	(openErr = svn_io_file_open(&tmpOut, pool.AllocString(tempOut), openFlags, APR_FPROT_OS_DEFAULT, pool.Handle)) ||
			(openErr = svn_io_file_open(&tmpErr, pool.AllocString(tempErr), openFlags, APR_FPROT_OS_DEFAULT, pool.Handle)) ||
			!tmpOut || !tmpErr)
		{
			if(openErr)
				throw SvnException::Create(openErr);

			throw gcnew System::IO::IOException("Can't create temporary files for diffing");
		}

		svn_opt_revision_t fromRev = from->GetSvnRevision(SvnRevision::Working, SvnRevision::Head);
		svn_opt_revision_t toRev = to->GetSvnRevision(SvnRevision::Working, SvnRevision::Head);

		IList<String^>^ diffArgs = args->DiffArguments;

		if(!diffArgs)
			diffArgs = safe_cast<IList<String^>^>(gcnew array<String^>(0));

		svn_error_t *r = svn_client_diff4(
			AllocArray(diffArgs, %pool),
			pool.AllocString(from->TargetName),
			&fromRev,
			pool.AllocString(to->TargetName),
			&toRev,
			(svn_depth_t)args->Depth,
			args->IgnoreAncestry,
			args->NoDeleted,
			args->IgnoreContentType,
			pool.AllocString(args->HeaderEncoding),
			tmpOut,
			tmpErr,
			CtxHandle,
			pool.Handle);

		apr_file_close(tmpOut);
		tmpOut = nullptr;

		result = gcnew DeleteOnCloseStream(tempOut, System::IO::FileMode::Open);		
		tempOut = nullptr;

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;

		if(tmpErr)
			apr_file_close(tmpErr);

		if(tmpOut)
			apr_file_close(tmpOut);

		if(tempErr && File::Exists(tempErr))
		{			
			File::Delete(tempErr);
		}
		if(tempOut && File::Exists(tempOut))
		{
			File::Delete(tempOut);
		}
	}
}

void SvnClient::Diff(SvnTarget^ source, SvnRevision^ from, SvnRevision^ to, [Out]FileStream^% result)
{
	if(!source)
		throw gcnew ArgumentNullException("source");
	else if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!to)
		throw gcnew ArgumentNullException("to");

	Diff(source, from, to, gcnew SvnDiffArgs(), result);
}

bool SvnClient::Diff(SvnTarget^ source, SvnRevision^ from, SvnRevision^ to, SvnDiffArgs^ args, [Out]FileStream^% result)
{
	if(!source)
		throw gcnew ArgumentNullException("source");
	else if(!from)
		throw gcnew ArgumentNullException("from");
	else if(!to)
		throw gcnew ArgumentNullException("to");

	EnsureState(SvnContextState::AuthorizationInitialized);

	String^ tempOut = System::IO::Path::GetTempFileName();
	String^ tempErr = System::IO::Path::GetTempFileName();

	if(_currentArgs)
		throw gcnew InvalidOperationException("Operation in progress; a client can handle only one command at a time");

	apr_file_t* tmpOut = nullptr;
	apr_file_t* tmpErr = nullptr;

	AprPool pool(%_pool);
	_currentArgs = args;

	try
	{
		apr_array_header_t *diff_options = nullptr;

		if(	apr_file_open(&tmpOut, pool.AllocPath(tempOut), APR_CREATE | APR_BUFFERED, APR_FPROT_OS_DEFAULT, pool.Handle) ||
			apr_file_open(&tmpErr, pool.AllocPath(tempErr), APR_CREATE | APR_BUFFERED, APR_FPROT_OS_DEFAULT, pool.Handle) ||
			!tmpOut || !tmpErr)
		{
			throw gcnew System::IO::IOException("Can't create temporary files for diffing");
		}

		svn_opt_revision_t pegRev = source->Revision->ToSvnRevision();
		svn_opt_revision_t fromRev = from->ToSvnRevision();
		svn_opt_revision_t toRev = to->ToSvnRevision();

		svn_error_t *r = svn_client_diff_peg4(
			diff_options,
			pool.AllocString(source->TargetName),
			&pegRev,
			&fromRev,
			&toRev,
			(svn_depth_t)args->Depth,
			args->IgnoreAncestry,
			args->NoDeleted,
			args->IgnoreContentType,
			pool.AllocString(args->HeaderEncoding),
			tmpOut,
			tmpErr,
			CtxHandle,
			pool.Handle);

		result = gcnew DeleteOnCloseStream(tempOut, System::IO::FileMode::Open);		
		tempOut = nullptr;

		return args->HandleResult(r);
	}
	finally
	{
		_currentArgs = nullptr;

		if(tmpErr)
			apr_file_close(tmpErr);

		if(tmpOut)
			apr_file_close(tmpOut);

		if(tempErr && File::Exists(tempErr))
		{			
			File::Delete(tempErr);
		}
		if(tempOut && File::Exists(tempOut))
		{
			File::Delete(tempOut);
		}
	}
}