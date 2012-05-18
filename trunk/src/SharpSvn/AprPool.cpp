// $Id$
//
// Copyright 2007-2008 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "stdafx.h"
#include "AprPool.h"

#include <svn_path.h>
#include <svn_pools.h>

#include "SvnTools.h"

#include "UnmanagedStructs.h" // Resolves linker warnings for opaque types

using namespace SharpSvn::Implementation;


[module: SuppressMessage("Microsoft.Usage", "CA2213:DisposableFieldsShouldBeDisposed", Scope="member", Target="SharpSvn.Implementation.AprPool.Dispose(System.Boolean):System.Void", MessageId="_tag")];
[module: SuppressMessage("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields", Scope="member", Target="SharpSvn.Implementation.AprPool.#StandardMemoryPressure")];

AprPool::AprPool(AprPool ^parentPool)
{
    if (!parentPool)
        throw gcnew ArgumentNullException("parentPool");

    _tag = gcnew AprPoolTag(parentPool->_tag);
    _parent = parentPool;
    _handle = svn_pool_create(parentPool->Handle);
    _destroyPool = true;

    if (!_noPressure)
        GC::AddMemoryPressure(_pressure = AprPool::StandardMemoryPressure);
}

AprPool::AprPool()
{
    _tag = gcnew AprPoolTag();
    _handle = svn_pool_create(const_cast<apr_pool_t*>(_ultimateParentPool));
    _destroyPool = true;
    if (!_noPressure)
        GC::AddMemoryPressure(_pressure = AprPool::StandardMemoryPressure);
}

AprPool::AprPool(apr_pool_t *handle, bool destroyPool)
{
    if (!handle)
        throw gcnew ArgumentNullException("handle");

    _handle = handle;
    _tag = gcnew AprPoolTag();
    _destroyPool = destroyPool;
    if (destroyPool && !_noPressure)
        GC::AddMemoryPressure(_pressure = AprPool::StandardMemoryPressure);
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
    if (!_handle)
        return;

    apr_pool_t* handle = _handle;
    _handle = nullptr;

    bool valid = (_tag != nullptr) && _tag->IsValid(); // Don't crash the finalizer; dont Destroy if parent is deleted

    if (valid)
    {
        delete _tag;
        _tag = nullptr;
    }

    if (_destroyPool)
    {
        try
        {
            if (valid)
                svn_pool_destroy(handle);
        }
        catch (...) {}
        finally
        {
            if (_pressure > 0)
                GC::RemoveMemoryPressure(_pressure);
        }
    }
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
        return "";
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
        return "";
}


const char* AprPool::AllocDirent(String^ value)
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

        if (!pData)
            throw gcnew InvalidOperationException();

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

        if ((len && pData[len-1] == '/') || strstr(pData+1, "//"))
            return svn_dirent_canonicalize(pData, Handle);
        else
        {
            char *pd = pData;
            while (pd = strstr(pd, "/."))
            {
                if (*(pd+2) == '/' || !*(pd+2))
                    return svn_dirent_canonicalize(pData, Handle);
                else
                    pd++;
            }
        }

        if (pData[0] >= 'a' && pData[0] <= 'z' && pData[1] == ':')
            pData[0] -= ('a' - 'A');

        return pData;
    }
    else
        return "";
}

const char* AprPool::AllocRelpath(String^ value)
{
    if (!value)
        throw gcnew ArgumentNullException("value");

    if (value->Length >= 1)
    {
        pin_ptr<unsigned char> pBytes = &(System::Text::Encoding::UTF8->GetBytes(value))[0];

        return svn_relpath_canonicalize((const char*)pBytes, Handle);
    }
    else
        return "";
}



const char* AprPool::AllocAbsoluteDirent(String^ value)
{
    return AllocDirent(SharpSvn::SvnTools::GetNormalizedFullPath(value));
}

const char* AprPool::AllocUri(Uri^ value)
{
    if (!value)
        return AllocUri((String^)nullptr);

    return AllocUri(SvnBase::UriToString(value));
}

const char* AprPool::AllocUri(String^ value)
{
    if (!value)
        value = "";

    if (value->Length >= 1)
    {
        cli::array<unsigned char>^ bytes = System::Text::Encoding::UTF8->GetBytes(value);

        for (int i = 0; i < bytes->Length; i++)
            if (bytes[i] == '\\')
                bytes[i] = '/';

        pin_ptr<unsigned char> pBytes = &bytes[0];
        const char* pcBytes = (const char*)static_cast<const unsigned char*>(pBytes);

        const char* resPath = svn_uri_canonicalize(pcBytes, Handle);

        if (resPath == pcBytes)
            resPath = apr_pstrdup(Handle, resPath);

        return resPath;
    }
    else
        return "";
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

const svn_string_t* AprPool::AllocPropertyValue(String^ value, String^ propertyName)
{
    const char* propName = AllocString(propertyName);

    if (svn_prop_is_boolean(propName))
        return AllocSvnString(value ? SVN_PROP_BOOLEAN_TRUE : "");
    else if (svn_prop_needs_translation(propName))
        return AllocUnixSvnString(value);
    else
        return AllocSvnString(value);
}

const svn_string_t* AprPool::AllocSvnString(array<Byte>^ bytes)
{
    if (!bytes)
        bytes = gcnew array<Byte>(0);

    svn_string_t* pStr = (svn_string_t*)AllocCleared(sizeof(svn_string_t));

    char* pChars = (char*)(Byte*)AllocCleared(bytes->Length+1);
    pStr->data = pChars;
    pStr->len = bytes->Length;

    if (bytes->Length > 0)
    {
        pin_ptr<Byte> pBytes = &bytes[0];
        memcpy(pChars, pBytes, bytes->Length);
    }

    return pStr;
}

class PoolAliveRef
{
private:
    int _magic;
    gcroot<Object^> _ref;
    bool _dispose;

public:
    PoolAliveRef(Object^ r, bool disposeOnCleanup)
    {
        _magic = 0xCACABA;
        _ref = r;
        _dispose = disposeOnCleanup;
    }

    static apr_status_t Cleanup(void* r)
    {
        PoolAliveRef* pr = reinterpret_cast<PoolAliveRef*>(r);

        if (pr && pr->_magic == 0xCACABA)
        {
            pr->_magic = 0;

            if (pr->_dispose)
            {
                IDisposable^ d = dynamic_cast<IDisposable^>((Object^)pr->_ref);

                delete pr;

                if (d)
                {
                    try
                    {
                        delete d; // Call Dispose();
                    }
                    catch(Exception^)
                    {}
                }
            }
            else
                delete pr;
        }
        return 0;
    }
};

void AprPool::KeepAlive(Object^ obj)
{
    if (!obj)
        throw gcnew ArgumentNullException("obj");

    Ensure();

    PoolAliveRef* r = new PoolAliveRef(obj, false);

    apr_pool_cleanup_register(Handle, reinterpret_cast<const void*>(r), PoolAliveRef::Cleanup, PoolAliveRef::Cleanup);
}

void AprPool::KeepAlive(IDisposable^ obj, bool disposeOnCleanup)
{
    if (!obj)
        throw gcnew ArgumentNullException("obj");

    Ensure();

    PoolAliveRef* r = new PoolAliveRef(obj, disposeOnCleanup);

    apr_pool_cleanup_register(Handle, reinterpret_cast<const void*>(r), PoolAliveRef::Cleanup, PoolAliveRef::Cleanup);
}
