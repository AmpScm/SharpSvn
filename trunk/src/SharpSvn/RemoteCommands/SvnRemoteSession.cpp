#include "stdafx.h"

#include "SvnAll.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types


using namespace SharpSvn;
using namespace SharpSvn::Implementation;

SvnRemoteSession::SvnRemoteSession()
	: _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
	_clientBaton = gcnew AprBaton<SvnRemoteSession^>(this);
	SVN_THROW(Init());
}

SvnRemoteSession::SvnRemoteSession(Uri^ sessionUri)
	: _pool(gcnew AprPool()), SvnClientContext(%_pool)
{
	_clientBaton = gcnew AprBaton<SvnRemoteSession^>(this);
	SVN_THROW(Init());
	if (!Open(sessionUri))
	{
		// Should never happen, unless somebody suppresses the error in Open
		throw gcnew InvalidOperationException();
	}
}

SvnRemoteSession::~SvnRemoteSession()
{
	_session = nullptr;
	delete _clientBaton;
}

svn_error_t * SvnRemoteSession::Init()
{
	CtxHandle->cancel_func;
	CtxHandle->cancel_baton;

	CtxHandle->progress_func;
	CtxHandle->progress_baton;

	return nullptr;
}

void SvnRemoteSession::Ensure()
{
	if (!_session)
		throw gcnew InvalidOperationException();
}