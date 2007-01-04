#include "stdafx.h"

#include "SvnAll.h"

using namespace QQn::Svn;

SvnClientContext::SvnClientContext(AprPool ^pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");

	_pool = pool;
	svn_client_ctx_t *ctx;

	svn_error_t *r = svn_client_create_context(&ctx, pool->Handle);

	if(r)
		throw gcnew SvnException(r);

	_ctx = ctx;
}

SvnClientContext::~SvnClientContext()
{
	_ctx = nullptr;
	_pool = nullptr;
}

svn_client_ctx_t *SvnClientContext::CtxHandle::get()
{
	if(!_ctx)
		throw gcnew ObjectDisposedException("SvnClientContext");

	_pool->Ensure();
	

	return _ctx;
}