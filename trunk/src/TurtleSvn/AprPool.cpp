#include "stdafx.h"
#include "AprPool.h"
#include "AprException.h"

#include <apr_pools.h>

using namespace QQn::Apr;

struct apr_allocator_t
{};

struct apr_pool_t
{};

AprPool::AprPool(apr_pool_t *handle)
{
	if(!handle)
		throw gcnew ArgumentNullException("handle");

	_handle = handle;
	_tag = gcnew AprPoolTag();
}

AprPool::~AprPool()
{
	Destroy();
}

AprPool::!AprPool()
{
	Destroy();
}

void AprPool::Destroy()
{
	_tag->Ensure();
	delete _tag; // Dispose

	apr_pool_t* handle = _handle;
	if(handle)
	{
		_handle = NULL;

		apr_pool_destroy(handle);
	}
}

AprPool::AprPool(QQn::Apr::AprPool ^parentPool)
{
	if(!parentPool)
		throw gcnew ArgumentNullException("parentPool");

	apr_pool_t* handle = NULL;
	apr_status_t hr = apr_pool_create(&handle, parentPool->Handle);

	if(hr)
		throw gcnew AprException(hr);
	else
		_handle = handle;

	_tag = gcnew AprPoolTag(parentPool->_tag);
}

AprPool::AprPool()
{
	apr_pool_t* handle = NULL;
	apr_status_t hr = apr_pool_create(&handle, NULL);

	if(hr)
		throw gcnew AprException(hr);
	else
		_handle = handle;

	_tag = gcnew AprPoolTag();
}