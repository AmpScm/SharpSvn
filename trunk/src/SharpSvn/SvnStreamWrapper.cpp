#include "stdafx.h"
#include "SvnAll.h"
#include "SvnStreamWrapper.h"

#include <svn_io.h>

using namespace SharpSvn::Apr;
using namespace SharpSvn;
using System::Runtime::InteropServices::Marshal;

SvnStreamWrapper::~SvnStreamWrapper()
{
	delete _streamBatton;
}

static svn_error_t *svnStreamRead(void *baton, char *buffer, apr_size_t *len)
{
	// Subversion:
	//		Handlers are obliged to complete a read or write
	//		to the maximum extent possible; thus, a short read with no
	//		associated error implies the end of the input stream, and a short
	//		write should never occur without an associated error.

	SvnStreamWrapper^ sw = AprBaton<SvnStreamWrapper^>::Get((IntPtr)baton);

	array<unsigned char>^ bytes = gcnew array<unsigned char>((int)*len);

	int count = sw->Stream->Read(bytes, 0, (int)*len);

	Marshal::Copy(bytes, 0, (IntPtr)buffer, count);
	*len = count;

	return nullptr;
}

static svn_error_t *svnStreamWrite(void *baton, const char *data, apr_size_t *len)
{
	// Subversion:
	//		Handlers are obliged to complete a read or write
	//		to the maximum extent possible; thus, a short read with no
	//		associated error implies the end of the input stream, and a short
	//		write should never occur without an associated error.

	SvnStreamWrapper^ sw = AprBaton<SvnStreamWrapper^>::Get((IntPtr)baton);

	array<unsigned char>^ bytes = gcnew array<unsigned char>((int)*len);

	System::Runtime::InteropServices::Marshal::Copy((IntPtr)const_cast<char*>(data), bytes, 0, bytes->Length);

	sw->Stream->Write(bytes, 0, bytes->Length);

	return nullptr;
}

static svn_error_t *svnStreamClose(void *baton)
{
	SvnStreamWrapper^ sw = AprBaton<SvnStreamWrapper^>::Get((IntPtr)baton);

	if(sw->Stream->CanWrite)
		sw->Stream->Flush();

	return nullptr;
}

void SvnStreamWrapper::Init(bool enableRead, bool enableWrite)
{
	_svnStream = svn_stream_create((void*)_streamBatton->Handle, _pool->Handle);

	if(!_svnStream)
		throw gcnew InvalidOperationException("Can't create svn stream");


	if(enableRead)
		svn_stream_set_read(_svnStream, svnStreamRead);
	if(enableWrite)
		svn_stream_set_write(_svnStream, svnStreamWrite);

	svn_stream_set_close(_svnStream, svnStreamClose);
}