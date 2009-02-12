#include "StdAfx.h"
#include "SvnAll.h"
#include "SvnDeltaTransform.h"

#include "SvnDeltaEditor.h"

using namespace SharpSvn;
using namespace SharpSvn::Delta;
using namespace SharpSvn::Implementation;

SvnDeltaTarget::SvnDeltaTarget()
{
	_targetBatton = gcnew AprBaton<SvnDeltaTarget^>(this);
}

SvnDeltaTarget::~SvnDeltaTarget()
{
	delete _targetBatton;
}

static svn_error_t* __cdecl 
svndeltatarget_window_handler(svn_txdelta_window_t *window, void *baton)
{
	SvnDeltaTarget^ target = AprBaton<SvnDeltaTarget^>::Get((IntPtr)baton);
	SvnDeltaWindowEventArgs^ ev = nullptr;
	try
	{
		if (target)
		{
			SVN_ERR(target->apply_window(window, ev));

			if (!window)
			{
				// Always call final_window from the top level handler to allow gathering properties
				// from multiple transformations
				target->final_window(ev ? gcnew SvnDeltaCompleteEventArgs(ev) : gcnew SvnDeltaCompleteEventArgs());
			}
		}

		return SVN_NO_ERROR;
	}
	catch(Exception^ e)
	{
		return SvnException::CreateExceptionSvnError("svndeltatarget_window_handler", e);
	}
	finally
	{
		if (ev)
			ev->Detach(false);
	}
}

void SvnDeltaTarget::AllocHandler(svn_txdelta_window_handler_t& handler, void*& handlerBaton, AprPool^ pool)
{
	if (!pool)
		throw gcnew ArgumentNullException("pool");

	pool->Ensure();

	handler = svndeltatarget_window_handler;
	handlerBaton = (void*)_targetBatton->Handle;
}

svn_error_t* SvnDeltaTarget::apply_window(svn_txdelta_window_t *window, SvnDeltaWindowEventArgs^% e)
{
	if (window && !e)
		e = gcnew SvnDeltaWindowEventArgs(window);
	
	if (window)
		OnDeltaWindow(e);

	return SVN_NO_ERROR;
}

void SvnDeltaTarget::final_window(SvnDeltaCompleteEventArgs^ e)
{
	OnDeltaComplete(e);
}

SvnDeltaFileTransform::SvnDeltaFileTransform(System::String ^baseFile, System::String ^toFile, bool verifyBase)
{
	if (String::IsNullOrEmpty(baseFile))
		throw gcnew ArgumentNullException("baseFile");
	else if (String::IsNullOrEmpty(toFile))
		throw gcnew ArgumentNullException("toFile");
	else if (!SvnBase::IsNotUri(baseFile))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "baseFile");
	else if (!SvnBase::IsNotUri(baseFile))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toFile");

	_baseFile = baseFile;
	_toFile = toFile;
	_verify = verifyBase;
}

SvnDeltaFileTransform::SvnDeltaFileTransform(System::String ^baseFile, System::String ^toFile)
{
	if (String::IsNullOrEmpty(baseFile))
		throw gcnew ArgumentNullException("baseFile");
	else if (String::IsNullOrEmpty(toFile))
		throw gcnew ArgumentNullException("toFile");
	else if (!SvnBase::IsNotUri(baseFile))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "baseFile");
	else if (!SvnBase::IsNotUri(baseFile))
		throw gcnew ArgumentException(SharpSvnStrings::ArgumentMustBeAPathNotAUri, "toFile");

	_baseFile = baseFile;
	_toFile = toFile;
	_verify = true;
}

SvnDeltaFileTransform::~SvnDeltaFileTransform()
{
	_pool = nullptr;
	_inStream = nullptr;
	_outStream = nullptr;
	_handler = nullptr;
	_handler_baton = nullptr;
	_checkSums = nullptr;
}

void SvnDeltaFileTransform::OnFileChange(SvnDeltaFileChangeEventArgs^ e)
{
	if (!e)
		throw gcnew ArgumentNullException("e");
	else if (_pool || !e->Pool || _inStream || _outStream)
		throw gcnew InvalidOperationException();

	_pool = e->Pool;
	AprPool tmpPool(_pool);

	svn_stream_t* inStream;
	svn_stream_t* outStream;

	SVN_THROW(svn_stream_open_readonly(&inStream, _pool->AllocPath(_baseFile), _pool->Handle, tmpPool.Handle));
	SVN_THROW(svn_stream_open_writable(&outStream, _pool->AllocPath(_toFile), _pool->Handle, tmpPool.Handle));

	if(_verify)
	{
		svn_checksum_t **checkSums = (svn_checksum_t**)_pool->AllocCleared(sizeof(svn_checksum_t*) * 2);
		_checkSums = checkSums;

		inStream = svn_stream_checksummed2(inStream, &checkSums[0], nullptr, svn_checksum_md5, true, _pool->Handle);
		outStream = svn_stream_checksummed2(outStream, nullptr, &checkSums[1], svn_checksum_md5, false, _pool->Handle);
	}

	_inStream = inStream;
	_outStream = outStream;

	svn_txdelta_window_handler_t handler;
	void* baton;

	svn_txdelta_apply(inStream, outStream, nullptr, nullptr, _pool->Handle, &handler, &baton);

	_handler = handler;
	_handler_baton = baton;
}

svn_error_t* SvnDeltaFileTransform::apply_window(svn_txdelta_window_t *window, SvnDeltaWindowEventArgs^% e)
{
	if (!_pool->IsValid() || !_inStream || !_outStream || !_handler)
		return SVN_NO_ERROR;

	SVN_ERR(_handler(window, _handler_baton));
	
	if (!window)
	{
		svn_stream_t* inStream = _inStream;
		svn_stream_t* outStream = _outStream;

		_inStream = _outStream = nullptr;
		//SVN_THROW(svn_stream_close(inStream));
		//SVN_THROW(svn_stream_close(outStream));

		if(_checkSums)
		{
			if (!e)
				e = gcnew SvnDeltaWindowEventArgs(_checkSums);
			else if (!e->_sums)
				e->_sums = _checkSums;
		}

		__super::apply_window(window, e);
	}

	return nullptr;
}