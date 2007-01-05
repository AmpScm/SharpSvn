#include "stdafx.h"
#include "AprPool.h"
#include "AprException.h"

#include <apr_pools.h>

using namespace TurtleSvn::Apr;

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
	if(_handle)
	{
		_tag->Ensure();
		delete _tag; // Dispose

		apr_pool_t* handle = _handle;
		if(handle)
		{
			_handle = nullptr;

			apr_pool_destroy(handle);
		}
	}
}

AprPool::AprPool(TurtleSvn::Apr::AprPool ^parentPool)
{
	if(!parentPool)
		throw gcnew ArgumentNullException("parentPool");

	apr_pool_t* handle = nullptr;
	apr_status_t hr = apr_pool_create(&handle, parentPool->Handle);

	if(hr)
		throw gcnew AprException(hr);
	else
		_handle = handle;

	_tag = gcnew AprPoolTag(parentPool->_tag);
	_parent = parentPool;
}

AprPool::AprPool()
{
	apr_pool_t* handle = nullptr;
	apr_status_t hr = apr_pool_create(&handle, nullptr);

	if(hr)
		throw gcnew AprException(hr);
	else
		_handle = handle;

	_tag = gcnew AprPoolTag();
}

void AprPool::Clear()
{
	_tag->Ensure();

	delete _tag;

	if(_parent)
		_tag = gcnew AprPoolTag(_parent->_tag);
	else
		_tag = gcnew AprPoolTag();

	apr_pool_clear(_handle);
}

void* AprPool::Alloc(size_t size)
{
	return apr_palloc(Handle, size);
}

void* AprPool::AllocCleared(size_t size)
{
	return apr_pcalloc(Handle, size);
}

const char* AprPool::AllocString(String^ value)
{
	if(!value)
		value = "";

	cli::array<unsigned char, 1>^ bytes = System::Text::Encoding::UTF8->GetBytes(value);

	char* pData = (char*)Alloc(bytes->Length+1);

	pin_ptr<unsigned char> pBytes = &bytes[0]; 

	if(pData && pBytes)
		memcpy(pData, pBytes, bytes->Length);

	pData[bytes->Length] = 0;

	return pData;
}

