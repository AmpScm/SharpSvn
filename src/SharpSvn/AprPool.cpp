// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "AprPool.h"

#include <svn_path.h>
#include <svn_pools.h>

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn::Implementation;


[module: SuppressMessage("Microsoft.Usage", "CA2213:DisposableFieldsShouldBeDisposed", Scope="member", Target="SharpSvn.Implementation.AprPool.Dispose(System.Boolean):System.Void", MessageId="_tag")];
[module: SuppressMessage("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields", Scope="member", Target="SharpSvn.Implementation.AprPool.#StandardMemoryPressure")];

AprPool::AprPool(apr_pool_t *handle, bool destroyPool)
{
	if (!handle)
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
	if (_handle)
	{
		if (_tag->IsValid()) // Don't crash the finalizer; dont Destroy if parent is deleted
		{
			delete _tag; // Dispose

			apr_pool_t* handle = _handle;
			if (handle)
			{
				_handle = nullptr;

				if (_destroyPool)
				{
					svn_pool_destroy(handle);

					GC::RemoveMemoryPressure(AprPool::StandardMemoryPressure);
				}
			}
		}
		else
			_handle = nullptr;
	}
}

AprPool::AprPool(AprPool ^parentPool)
{
	if (!parentPool)
		throw gcnew ArgumentNullException("parentPool");

	_tag = gcnew AprPoolTag(parentPool->_tag);
	_parent = parentPool;
	_handle = svn_pool_create(parentPool->Handle);
	_destroyPool = true;
	GC::AddMemoryPressure(AprPool::StandardMemoryPressure);
}

AprPool::AprPool()
{
	_tag = gcnew AprPoolTag();
	_handle = svn_pool_create(nullptr);
	_destroyPool = true;
	GC::AddMemoryPressure(AprPool::StandardMemoryPressure);
}

void AprPool::Clear()
{
	_tag->Ensure();

	delete _tag;

	if (_parent)
		_tag = gcnew AprPoolTag(_parent->_tag);
	else
		_tag = gcnew AprPoolTag();

	svn_pool_clear(_handle);
}

void* AprPool::Alloc(size_t size)
{
	void *p = apr_palloc(Handle, size);

	if (!p)
		throw gcnew ArgumentException("apr_palloc returned null; We have crashed before you see this (See svn sourcecode)", "size");

	return p;
}

void* AprPool::AllocCleared(size_t size)
{
	void *p = apr_pcalloc(Handle, size);

	if (!p)
		throw gcnew ArgumentException("apr_pcalloc returned null; We have crashed before you see this (See svn sourcecode)", "size");

	return p;
}

const char* AprPool::AllocString(String^ value)
{
	if (!value)
		value = "";

	if (value->Length >= 1)
	{
		cli::array<unsigned char>^ bytes = System::Text::Encoding::UTF8->GetBytes(value);

		char* pData = (char*)Alloc(bytes->Length+1);

		pin_ptr<unsigned char> pBytes = &bytes[0];

		if (pData && pBytes)
			memcpy(pData, pBytes, bytes->Length);

		pData[bytes->Length] = 0;

		return pData;
	}
	else
		return (const char*)AllocCleared(1);
}

const char* AprPool::AllocUnixString(String^ value)
{
	if (!value)
		value = "";

	if (value->Length >= 1)
	{
		cli::array<unsigned char>^ bytes = System::Text::Encoding::UTF8->GetBytes(value);

		char* pData = (char*)Alloc(bytes->Length+1);

		pin_ptr<unsigned char> pBytes = &bytes[0];

		if (pData && pBytes)
			memcpy(pData, pBytes, bytes->Length);

		pData[bytes->Length] = 0;

		char *pFrom;
		char *pTo;

		pFrom = pTo = pData;

		while (*pFrom)
		{
			switch(*pFrom)
			{
			case '\r':
				*pTo++ = '\n';
				if (*(++pFrom) == '\n')
					pFrom++;
				break;
			case '\n':
				*pTo++ = '\n';
				if (*(++pFrom) == '\r')
					pFrom++;
				break;

			default:
				*pTo++ = *pFrom++;
			}
		}

		*pTo = 0;

		return pData;
	}
	else
		return (const char*)AllocCleared(1);
}


const char* AprPool::AllocPath(String^ value)
{
	if (!value)
		throw gcnew ArgumentNullException("value");

	if (value->Length >= 1)
	{
		cli::array<unsigned char>^ bytes = System::Text::Encoding::UTF8->GetBytes(value);

		int len = bytes->Length;

		while (len && ((bytes[len-1] == System::IO::Path::DirectorySeparatorChar) || bytes[len-1] == System::IO::Path::AltDirectorySeparatorChar))
			len--;

		char* pData = (char*)Alloc(len+1);

		pin_ptr<unsigned char> pBytes = &bytes[0];

		if (pData && pBytes)
		{
			memcpy(pData, pBytes, len);

			// Should match: svn_path_internal_style() implementation, but doesn't copy an extra time
			for (int i = 0; i < len; i++)
				if ((pData[i] == System::IO::Path::DirectorySeparatorChar) || (pData[i] == System::IO::Path::AltDirectorySeparatorChar))
					pData[i] = '/';
		}

		pData[len] = 0;

		return pData;
	}
	else
		return (const char*)AllocCleared(1);
}

const char* AprPool::AllocCanonical(Uri^ value)
{
	if(!value)
		return AllocCanonical((String^)nullptr);

	return AllocCanonical(value->ToString());
}

const char* AprPool::AllocCanonical(String^ value)
{
	if (!value)
		value = "";

	if (value->Length >= 1)
	{
		cli::array<unsigned char>^ bytes = System::Text::Encoding::UTF8->GetBytes(value);

		pin_ptr<unsigned char> pBytes = &bytes[0];
		const char* pcBytes = (const char*)static_cast<const unsigned char*>(pBytes);

		const char* resPath = svn_path_canonicalize(pcBytes, Handle);

		if (resPath == pcBytes)
			resPath = apr_pstrdup(Handle, resPath);

		return resPath;
	}
	else
		return (const char*)AllocCleared(1);
}

const svn_string_t* AprPool::AllocSvnString(String^ value)
{
	if (!value)
		value = "";

	svn_string_t* pStr = (svn_string_t*)AllocCleared(sizeof(svn_string_t));

	pStr->data = AllocString(value);
	pStr->len = strlen(pStr->data);

	return pStr;
}

const svn_string_t* AprPool::AllocUnixSvnString(String^ value)
{
	if (!value)
		value = "";

	svn_string_t* pStr = (svn_string_t*)AllocCleared(sizeof(svn_string_t));

	pStr->data = AllocUnixString(value);
	pStr->len = strlen(pStr->data);

	return pStr;
}


const svn_string_t* AprPool::AllocSvnString(array<Byte>^ bytes)
{
	if (!bytes)
		bytes = gcnew array<Byte>(0);

	svn_string_t* pStr = (svn_string_t*)AllocCleared(sizeof(svn_string_t));

	char* pChars = (char*)(Byte*)AllocCleared(bytes->Length+1);
	pStr->data = pChars;
	pStr->len = bytes->Length;

	pin_ptr<Byte> pBytes = &bytes[0];
	memcpy(pChars, pBytes, bytes->Length);

	return pStr;
}
