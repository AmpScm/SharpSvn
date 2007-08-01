#include "stdafx.h"
#include "AprPool.h"
#include "AprException.h"

#include <apr_pools.h>
#include <svn_path.h>

using namespace SharpSvn::Apr;

struct apr_allocator_t
{};

struct apr_pool_t
{};

AprPool::AprPool(apr_pool_t *handle, bool destroyPool)
{
	if(!handle)
		throw gcnew ArgumentNullException("handle");

	_handle = handle;
	_tag = gcnew AprPoolTag();
	_destroyPool = destroyPool;
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
		if(_tag->IsValid()) // Don't crash the finalizer; dont Destroy if parent is deleted
		{
			delete _tag; // Dispose

			apr_pool_t* handle = _handle;
			if(handle)
			{
				_handle = nullptr;

				if(_destroyPool)
					apr_pool_destroy(handle);
			}
		}
		else
			_handle = nullptr;
	}
}

AprPool::AprPool(SharpSvn::Apr::AprPool ^parentPool)
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
	_destroyPool = true;
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
	_destroyPool = true;
}

AprPool^ AprPool::Attach(apr_pool_t *handle, bool destroyPool)
{
	if(!handle)
		throw gcnew ArgumentNullException("handle");

	return gcnew AprPool(handle, destroyPool);
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
	void *p = apr_palloc(Handle, size);

	if(!p)
		throw gcnew OutOfMemoryException("apr_palloc returned null");

	return p;
}

void* AprPool::AllocCleared(size_t size)
{
	void *p = apr_pcalloc(Handle, size);

	if(!p)
		throw gcnew OutOfMemoryException("apr_pcalloc returned null");

	return p;
}

void* AprPool::Alloc(size_t size, apr_pool_t *pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");

	void* p = apr_palloc(pool, size);

	if(!p)
		throw gcnew OutOfMemoryException("apr_palloc returned null");

	return p;
}

void* AprPool::AllocCleared(size_t size, apr_pool_t *pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");

	void* p = apr_pcalloc(pool, size);

	if(!p)
		throw gcnew OutOfMemoryException("apr_pcalloc returned null");

	return p;
}

const char* AprPool::AllocString(String^ value)
{
	if(!value)
		value = "";

	if(value->Length >= 1)
	{
		cli::array<unsigned char, 1>^ bytes = System::Text::Encoding::UTF8->GetBytes(value);

		char* pData = (char*)Alloc(bytes->Length+1);

		pin_ptr<unsigned char> pBytes = &bytes[0]; 

		if(pData && pBytes)
			memcpy(pData, pBytes, bytes->Length);

		pData[bytes->Length] = 0;

		return pData;
	}
	else
		return (const char*)AllocCleared(1);
}

const char* AprPool::AllocPath(String^ value)
{
	if(!value)
		value = "";

	if(value->Length >= 1)
	{
		cli::array<unsigned char, 1>^ bytes = System::Text::Encoding::UTF8->GetBytes(value);

		char* pData = (char*)Alloc(bytes->Length+1);

		pin_ptr<unsigned char> pBytes = &bytes[0]; 

		if(pData && pBytes)
		{
			memcpy(pData, pBytes, bytes->Length);

			// Should match: svn_path_internal_style() implementation, but doesn't copy an extra time
			for(int i = 0; i < bytes->Length; i++)
				if(pData[i] == '\\')
					pData[i] = '/';
		}

		pData[bytes->Length] = 0;

		return pData;
	}
	else
		return (const char*)AllocCleared(1);
}

const svn_string_t* AprPool::AllocSvnString(String^ value)
{
	if(!value)
		value = "";

	svn_string_t* pStr = (svn_string_t*)AllocCleared(sizeof(svn_string_t));

	pStr->data = AllocString(value);
	pStr->len = strlen(pStr->data);

	return pStr;
}

const svn_string_t* AprPool::AllocSvnString(array<char>^ bytes)
{
	if(!bytes)
		bytes = gcnew array<char>(0);

	svn_string_t* pStr = (svn_string_t*)AllocCleared(sizeof(svn_string_t));

	char* pChars = (char*)AllocCleared(bytes->Length+1);
	pStr->data = pChars;
	pStr->len = bytes->Length;

	pin_ptr<char> pBytes = &bytes[0]; 
	memcpy(pChars, pBytes, bytes->Length);

	return pStr;
}


const char* AprPool::AllocString(String^ value, apr_pool_t *pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");
	
	if(!value)
		value = "";

	if(value->Length > 0)
	{
		cli::array<unsigned char, 1>^ bytes = System::Text::Encoding::UTF8->GetBytes(value);

		char* pData = (char*)Alloc(bytes->Length+1, pool);

		pin_ptr<unsigned char> pBytes = &bytes[0]; 

		if(pData && pBytes)
			memcpy(pData, pBytes, bytes->Length);

		pData[bytes->Length] = 0;

		return pData;
	}
	else
		return (const char*)AllocCleared(1, pool);
}

const char* AprPool::AllocPath(String^ value, apr_pool_t *pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");
	
	if(!value)
		value = "";

	if(value->Length > 0)
	{
		cli::array<unsigned char, 1>^ bytes = System::Text::Encoding::UTF8->GetBytes(value);

		char* pData = (char*)Alloc(bytes->Length+1, pool);

		pin_ptr<unsigned char> pBytes = &bytes[0]; 

		if(pData && pBytes)
		{
			memcpy(pData, pBytes, bytes->Length);

			// Should match: svn_path_internal_style() implementation, but doesn't copy an extra time
			for(int i = 0; i < bytes->Length; i++)
				if(pData[i] == '\\')
					pData[i] = '/';
		}

		pData[bytes->Length] = 0;

		return pData;
	}
	else
		return (const char*)AllocCleared(1, pool);
}

const svn_string_t* AprPool::AllocSvnString(String^ value, apr_pool_t *pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");

	if(!value)
		value = "";

	svn_string_t* pStr = (svn_string_t*)AllocCleared(sizeof(svn_string_t), pool);

	pStr->data = AllocString(value, pool);
	pStr->len = strlen(pStr->data);

	return pStr;
}

const svn_string_t* AprPool::AllocSvnString(array<char>^ bytes, apr_pool_t *pool)
{
	if(!pool)
		throw gcnew ArgumentNullException("pool");
	
	if(!bytes)
		bytes = gcnew array<char>(0);

	svn_string_t* pStr = (svn_string_t*)Alloc(sizeof(svn_string_t), pool);

	char *pChars = (char*)AllocCleared(bytes->Length+1, pool);
	pStr->data = pChars;
	pStr->len = bytes->Length;

	pin_ptr<char> pBytes = &bytes[0]; 
	memcpy(pChars, pBytes, bytes->Length);

	return pStr;
}

