#include "stdafx.h"

#include "SvnAll.h"
#include <svn_config.h>

using namespace TurtleSvn;

struct apr_hash_t
{};


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
	_authentication = gcnew TurtleSvn::Security::SvnAuthentication();
}

SvnClientContext::SvnClientContext(SvnClientContext ^fromContext)
{
	if(!fromContext)
		throw gcnew ArgumentNullException("fromContext");
	
	fromContext->_pool->Ensure();
	_pool = fromContext->_pool;

	svn_client_ctx_t *ctx;

	svn_error_t *r = svn_client_create_context(&ctx, _pool->Handle);

	if(r)
		throw gcnew SvnException(r);

	_ctx = ctx;
	_authentication = gcnew TurtleSvn::Security::SvnAuthentication();
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

void SvnClientContext::EnsureState(SvnContextState state)
{
	if(state < State)
		return;

	if(State < SvnContextState::ConfigLoaded && state >= SvnContextState::ConfigLoaded)
	{
		LoadConfigurationDefault();

		System::Diagnostics::Debug::Assert(State == SvnContextState::ConfigLoaded);
	}
}

void SvnClientContext::LoadConfiguration(String ^path, bool ensurePath)
{
	if(State >= SvnContextState::ConfigLoaded)
		throw gcnew InvalidOperationException("Configuration already loaded");

	if(String::IsNullOrEmpty(path))
		path = nullptr;

	AprPool^ tmpPool = gcnew AprPool(_pool);
	try
	{
		const char* szPath = path ? tmpPool->AllocString(path) : nullptr;

		svn_error_t* err = nullptr;

		if(ensurePath)
		{
			err = svn_config_ensure(szPath, tmpPool->Handle);

			if(err)
				throw gcnew SvnException(err);
		}

		apr_hash_t* cfg = nullptr;
		err = svn_config_get_config(&cfg, szPath, _pool->Handle);

		if(err)
			throw gcnew SvnException(err);

		CtxHandle->config = cfg;

		_contextState = SvnContextState::ConfigLoaded;
	}
	finally
	{
		delete tmpPool;
	}
}

void SvnClientContext::LoadConfiguration(String ^path)
{
	LoadConfiguration(path, false);
}

void SvnClientContext::LoadConfigurationDefault()
{
	LoadConfiguration(nullptr, true);
}

void SvnClientContext::MergeConfiguration(String^ path)
{
	if(!path)
		throw gcnew ArgumentNullException("path");

	if(State < SvnContextState::ConfigLoaded)
		LoadConfigurationDefault();

	AprPool^ tmpPool = gcnew AprPool(_pool);
	try
	{
		const char* szPath = tmpPool->AllocString(path);

		svn_error_t* err = svn_config_get_config(&CtxHandle->config, szPath, _pool->Handle);

		if(err)
			throw gcnew SvnException(err);
	}
	finally
	{
		delete tmpPool;
	}
}