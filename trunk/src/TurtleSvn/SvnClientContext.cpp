#include "stdafx.h"

#include "SvnAll.h"

using namespace QQn::Svn;

SvnClientContext::SvnClientContext(AprPool ^pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");

	svn_client_ctx_t *ctx;

	svn_error_t *r = svn_client_create_context(&ctx, pool->Handle);

	if(r)
		throw gcnew SvnException(r);
}

SvnClientContext::~SvnClientContext()
{
	_handle = NULL;
}