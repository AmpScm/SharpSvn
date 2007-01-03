#include "stdafx.h"

#include "SvnClient.h"

using namespace QQn::Svn;

SvnClient::SvnClient()
	: _pool(gcnew AprPool()), SvnClientContext(_pool)
{
}

SvnClient::SvnClient(AprPool^ pool)
	: _pool(gcnew AprPool(pool)), SvnClientContext(_pool)
{
}

SvnClient::~SvnClient()
{
	AprPool^ pool = _pool;
	if(pool)
	{
		_pool = nullptr;

		if(!pool->IsDisposed)
			delete pool;
	}
}
