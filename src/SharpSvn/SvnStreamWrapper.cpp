// Copyright 2007-2008 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "stdafx.h"
#include "SvnAll.h"
#include "SvnStreamWrapper.h"

#include <svn_io.h>

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using System::Runtime::InteropServices::Marshal;

SvnStreamWrapper::~SvnStreamWrapper()
{
    try
    {
        if (_written)
            _stream->Flush();
    }
    finally
    {
        delete _streamBaton;
    }
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
    if (*len == 0)
        return nullptr;

	// Subversion:
	//		Handlers are obliged to complete a read or write
	//		to the maximum extent possible; thus, a short read with no
	//		associated error implies the end of the input stream, and a short
	//		write should never occur without an associated error.

	SvnStreamWrapper^ sw = AprBaton<SvnStreamWrapper^>::Get((IntPtr)baton);

	array<unsigned char>^ bytes = gcnew array<unsigned char>((int)*len);

	System::Runtime::InteropServices::Marshal::Copy((IntPtr)const_cast<char*>(data), bytes, 0, bytes->Length);

	sw->Stream->Write(bytes, 0, bytes->Length);
    sw->_written = true;

	return nullptr;
}

static svn_error_t *svnStreamClose(void *baton)
{
	SvnStreamWrapper^ sw = AprBaton<SvnStreamWrapper^>::Get((IntPtr)baton);

	if (sw->_written)
    {
        sw->_written = false;
		sw->Stream->Flush();
    }

	return nullptr;
}

void SvnStreamWrapper::Init(bool enableRead, bool enableWrite)
{
	_svnStream = svn_stream_create((void*)_streamBaton->Handle, _pool->Handle);

	if (!_svnStream)
		throw gcnew InvalidOperationException("Can't create svn stream");


	if (enableRead)
		svn_stream_set_read(_svnStream, svnStreamRead);
	if (enableWrite)
		svn_stream_set_write(_svnStream, svnStreamWrite);

	svn_stream_set_close(_svnStream, svnStreamClose);
}
