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

#include "svn_dso.h"
#include "svn_utf.h"
#include "svn_cache_config.h"
#include "private/svn_cache.h"

#include "UnmanagedStructs.h"

using System::Text::StringBuilder;
using System::IO::Path;
using System::Globalization::CultureInfo;
using namespace SharpSvn;

static SvnBase::SvnBase()
{
    System::Reflection::AssemblyName^ name = gcnew System::Reflection::AssemblyName(SvnBase::typeid->Assembly->FullName);

    String^ platform = Environment::OSVersion->Platform.ToString();

    // This part is appended to something like "SVN/1.5.0 (SharpSvn:branch/1.5.X@12345) WIN32/" to form the user agent on web requests

    _clientName = Environment::OSVersion->Version->ToString(2) +
        " " + name->Name + "/" + name->Version->ToString();

    _clientNames->Add(platform);
    _clientNames->Add(name->Name);

    EnsureLoaded();
}

void SvnBase::EnsureLoaded()
{
    static volatile LONG ensurer = 0;

    if (::InterlockedCompareExchange(&ensurer, 1, 0) < 2)
    {
        System::Threading::Monitor::Enter(_ensurerLock);
        try
        {
            if (!_aprInitialized)
            {
                _aprInitialized = true;

                if (apr_initialize()) // First
                    throw gcnew InvalidOperationException();

                SVN_THROW(svn_dso_initialize2()); // Before first pool

                apr_pool_t* pool = svn_pool_create(nullptr);

                apr_allocator_t* allocator = apr_pool_allocator_get(pool);

                if (allocator)
                {
                    apr_allocator_max_free_set(allocator, 1); // Keep a maximum of 1 free block
                }

                svn_utf_initialize2(TRUE, pool);

                // Enable a very limited cache for fsfs file:/// access to avoid errors with 14.x
                {
                    svn_cache_config_t settings = *svn_cache_config_get();
                    settings.cache_size = 2 * 1024 * 1024;
                    settings.file_handle_count = 16;
                    settings.single_threaded = FALSE;
                    svn_cache_config_set(&settings);
                    svn_cache__get_global_membuffer_cache();
                }

                if (getenv("SVN_ASP_DOT_NET_HACK"))
                {
                    svn_wc_set_adm_dir("_svn", pool);
                }

                SVN_THROW(svn_ra_initialize(pool));

                _admDir = svn_wc_get_adm_dir(pool);

                InstallAbortHandler();
                InstallSslDialogHandler();

                int r = libssh2_init(0);
                if (r)
                    throw gcnew InvalidOperationException("Can't initialize libssh2");

                LONG v = ::InterlockedExchange(&ensurer, 2);

                System::Diagnostics::Debug::Assert(v == 1);
            }
        }
        finally
        {
            System::Threading::Monitor::Exit(_ensurerLock);
        }
    }
}

SvnBase::SvnBase()
{
}

AprPool^ SvnBase::SmallThreadPool::get()
{
    if (!_threadPool || !_threadPool->IsValid()) // Recreate if disposed for some reason
        _threadPool = gcnew AprPool();

    return _threadPool;
}

bool SvnBase::IsValidReposUri(Uri^ uri)
{
    if (!uri)
        throw gcnew ArgumentNullException("uri");

    if (!uri->IsAbsoluteUri)
        return false;
    else if (String::IsNullOrEmpty(uri->Scheme))
        return false;

    return true;
}


bool SvnBase::IsNotUri(String ^path)
{
    if (String::IsNullOrEmpty(path))
        return false;

    // Use the same stupid algorithm subversion uses to choose between Uri's and paths
    for (int i = 0; i < path->Length; i++)
    {
        wchar_t c = path[i];
        switch(c)
        {
        case '\\':
        case '/':
            return true;
        case ':':
            if (i < path->Length-2)
            {
                if ((path[i+1] == '/') && (path[i+2] == '/'))
                    return false;
            }
            return true;
        case '+':
        case '-':
        case '_':
            break;
        default:
            if (!wchar_t::IsLetter(c))
                return true;
        }
    }
    return true;
}

bool SvnBase::IsValidRelpath(String^ path)
{
    if (!path)
        return false;

    if (path->Length == 0)
        return true;

    if (path[0] == '/' || path[path->Length-1] == '/' || path->IndexOf('\\') >= 0)
        return false;

    // TODO: Add full verification

    return true;
}

String^ SvnBase::RemoveDoubleSlashes(String^ input)
{
    int n;

    while (0 <= (n = input->IndexOf("//", StringComparison::Ordinal)))
        input = input->Remove(n, 1);

    return input;
}

static bool ContainsUpper(String^ value)
{
    if (String::IsNullOrEmpty(value))
        return false;

    for (int i = 0; i < value->Length; i++)
        if (Char::IsUpper(value, i))
            return true;

    return false;
}

[module: SuppressMessage("Microsoft.Globalization", "CA1308:NormalizeStringsToUppercase", Scope="member", Target="SharpSvn.Implementation.SvnBase.#CanonicalizeUri(System.Uri)")];

Uri^ SvnBase::CanonicalizeUri(Uri^ uri)
{
    if (!uri)
        throw gcnew ArgumentNullException("uri");
    else if (!uri->IsAbsoluteUri)
        throw gcnew ArgumentException(SharpSvnStrings::UriIsNotAbsolute, "uri");

    String^ path = uri->GetComponents(UriComponents::Path, UriFormat::SafeUnescaped);

    bool schemeOk = !ContainsUpper(uri->Scheme) && !ContainsUpper(uri->Host);

    if (schemeOk && (path->Length == 0 || (path[path->Length -1] != '/' && path->IndexOf('\\') < 0) && !path->Contains("//"))
    && !(uri->IsFile && (uri->IsUnc ? String::Equals("localhost", uri->Host) : Char::IsLower(uri->LocalPath, 0))))
        return uri;

    String^ components = uri->GetComponents(UriComponents::SchemeAndServer | UriComponents::UserInfo, UriFormat::SafeUnescaped);
    if (!schemeOk)
    {
        int nStart = components->IndexOf(':');

        if (nStart > 0)
        {
            // Subversion 1.6 will require scheme and hostname in lowercase
            for (int i = 0; i < nStart; i++)
                if (!Char::IsLower(components, i))
                {
                    components = components->Substring(0, nStart)->ToLowerInvariant() + components->Substring(nStart);
                    break;
                }

                int nAt = components->IndexOf('@', nStart);

                if (nAt < 0)
                    nAt = nStart + 2;
                else
                    nAt++;

                for (int i = nAt; i < components->Length; i++)
                    if (!Char::IsLower(components, i))
                    {
                        components = components->Substring(0, nAt) + components->Substring(nAt)+1;
                        break;
                    }
        }
    }

    // Create a new uri with all / and \ characters at the end removed
    Uri^ root;
    Uri^ suffix;

    if (!Uri::TryCreate(components, UriKind::Absolute, root))
        throw gcnew ArgumentException("Invalid Uri value in scheme or server", "uri");

    String^ part = RemoveDoubleSlashes("/" + path->TrimEnd(System::IO::Path::DirectorySeparatorChar, System::IO::Path::AltDirectorySeparatorChar));

    if (root->IsFile)
    {
        if (!root->IsUnc || root->Host == "localhost")
        {
            if(part->Length >= 3 && part[0] =='/' && part[2] == ':' && part[1] >= 'a' && part[1] <= 'z')
            {
                part = "/" + Char::ToUpperInvariant(part[1]) + part->Substring(2);

                if(part->Length == 3)
                    part += "/";
            }

            if (root->IsUnc)
        part = "//localhost/" + part->TrimStart('/');
        }
        else
        {
            part = part->TrimStart('/');
        }
    }

    if (!Uri::TryCreate(part, UriKind::Relative, suffix))
        throw gcnew ArgumentException("Invalid Uri value in path", "uri");

    Uri^ result;
    if (Uri::TryCreate(root, suffix, result))
        return result;
    else
        return uri;
}

Uri^ SvnBase::PathToUri(String^ path)
{
    if (String::IsNullOrEmpty(path))
        throw gcnew ArgumentNullException("path");

    StringBuilder^ sb = gcnew StringBuilder();
    Uri^ result;

    bool afterFirst = false;

    auto path_splitters = gcnew array<wchar_t>(2) {
        Path::DirectorySeparatorChar,
            Path::AltDirectorySeparatorChar
    };

    for each (String^ p in path->Split(path_splitters))
    {
        if (afterFirst)
            sb->Append((Char)'/');
        else
            afterFirst = true;

        sb->Append(Uri::EscapeDataString(p));
    }

    if (Uri::TryCreate(sb->ToString(), UriKind::Relative, result))
        return result;

    throw gcnew ArgumentException("Path is not convertible to uri part", "path");
}

/*String^ SvnBase::CanonicalizePath(String^ path)
{
if (!path)
throw gcnew ArgumentNullException("path");

if (path->Length > 0 && ((path[path->Length-1] == System::IO::Path::DirectorySeparatorChar) || (path[path->Length-1] == System::IO::Path::AltDirectorySeparatorChar)))
return path->TrimEnd(System::IO::Path::DirectorySeparatorChar, System::IO::Path::AltDirectorySeparatorChar);

return path;
}*/

String^ SvnBase::Utf8_PathPtrToString(const char *ptr, AprPool^ pool)
{
    if (!ptr || !pool)
        return nullptr;

    if (!*ptr)
        return "";

    return Utf8_PtrToString(svn_dirent_local_style(ptr, pool->Handle));
}


String^ SvnBase::Utf8_PtrToString(const char *ptr)
{
    if (!ptr)
        return nullptr;

    if (!*ptr)
        return "";

    return Utf8_PtrToString(ptr, (int)::strlen(ptr));
}

String^ SvnBase::Utf8_PtrToString(const char *ptr, int length)
{
    if (!ptr || length < 0)
        return nullptr;

    if (!*ptr)
        return "";

    return gcnew String(ptr, 0, length, System::Text::Encoding::UTF8);
}

Uri^ SvnBase::Utf8_PtrToUri(const char *ptr, SvnNodeKind nodeKind)
{
    if (!ptr)
        return nullptr;

    String^ url = Utf8_PtrToString(ptr);

    if (!url)
        return nullptr;
    else if (nodeKind == SvnNodeKind::Directory && !url->EndsWith("/", StringComparison::Ordinal))
        url += "/";

    Uri^ uri;

    if (Uri::TryCreate(url, UriKind::Absolute, uri))
        return uri;
    else
        return nullptr;
}


array<Byte>^ SvnBase::PtrToByteArray(const char* ptr, int length)
{
    if (!ptr || length < 0)
        return nullptr;

    array<Byte>^ bytes = gcnew array<Byte>(length);

    if (length > 0)
    {
        pin_ptr<Byte> pBytes = &bytes[0];

        memcpy(pBytes, ptr, length);
    }

    return bytes;
}

Object^ SvnBase::PtrToStringOrByteArray(const char* ptr, int length)
{
    if (!ptr || length < 0)
        return nullptr;
    else if (length == 0)
        return "";

    for(int i = 0; i < length; i++)
    {
        if(!ptr[i])
        {
            // A string that contains a 0 byte can never be valid Utf-8
            return SvnBase::PtrToByteArray(ptr, length);
        }
    }

    try
    {
        return Utf8_PtrToString(ptr, length);
    }
    catch(System::Text::DecoderFallbackException^)
    {
        return SvnBase::PtrToByteArray(ptr, length);
    }
    catch(ArgumentException^)
    {
        return SvnBase::PtrToByteArray(ptr, length);
    }
}

DateTime SvnBase::DateTimeFromAprTime(apr_time_t aprTime)
{
    if (aprTime == 0)
        return DateTime::MinValue;
    else
    {
        __int64 aprTimeBase = DateTime(1970,1,1).Ticks;

        return System::DateTime(aprTime*10 + aprTimeBase, DateTimeKind::Utc);
    }
}

apr_time_t SvnBase::AprTimeFromDateTime(DateTime time)
{
    __int64 aprTimeBase = DateTime(1970,1,1).Ticks;

    if (time != DateTime::MinValue)
        return (time.ToUniversalTime().Ticks - aprTimeBase) / 10L;
    else
        return 0;
}

static SvnHandleBase::SvnHandleBase()
{
    SvnBase::EnsureLoaded();
}

SvnHandleBase::SvnHandleBase()
{
}

ref class SvnCopyTargetMarshaller : public IItemMarshaller<SvnTarget^>
{
public:
    SvnCopyTargetMarshaller()
    {}

    property int ItemSize
    {
        virtual int get()
        {
            return sizeof(svn_client_copy_source_t*);
        }
    }

    virtual void Write(SvnTarget^ value, void* ptr, AprPool^ pool)
    {
        svn_client_copy_source_t **src = (svn_client_copy_source_t**)ptr;
        *src = (svn_client_copy_source_t *)pool->AllocCleared(sizeof(svn_client_copy_source_t));

    (*src)->path = value->AllocAsString(pool, true);
        (*src)->revision = value->GetSvnRevision(SvnRevision::Working, SvnRevision::Head)->AllocSvnRevision(pool);
        (*src)->peg_revision = value->GetSvnRevision(SvnRevision::Working, SvnRevision::Head)->AllocSvnRevision(pool);
    }

    virtual SvnTarget^ Read(const void* ptr, AprPool^ pool)
    {
        UNUSED_ALWAYS(ptr);
        UNUSED_ALWAYS(pool);
        //const char** ppcStr = (const char**)ptr;

        return nullptr;
    }
};

generic<typename TSvnTarget> where TSvnTarget : SvnTarget
apr_array_header_t *SvnBase::AllocCopyArray(ICollection<TSvnTarget>^ targets, AprPool^ pool)
{
    if (!targets)
        throw gcnew ArgumentNullException("targets");

    for each (SvnTarget^ s in targets)
    {
        if (!s)
            throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targets");
    }
    AprArray<SvnTarget^, SvnCopyTargetMarshaller^>^ aprTargets = gcnew AprArray<SvnTarget^, SvnCopyTargetMarshaller^>(targets, pool);

    return aprTargets->Handle;
}

generic<typename T>
array<T>^ SvnBase::ExtendArray(array<T>^ from, T value)
{
    array<T>^ to = gcnew array<T>(from ? from->Length + 1 : 1);

    if(from)
        from->CopyTo(to, 0);

    to[to->Length-1] = value;

    return to;
}

generic<typename T>
array<T>^ SvnBase::ExtendArray(array<T>^ from, array<T>^ values)
{
    return ExtendArray(from, safe_cast<ICollection<T>^>(values));
}

generic<typename T>
array<T>^ SvnBase::ExtendArray(array<T>^ from, ICollection<T>^ values)
{
    int n = 0;
    int c = 0;
    if (from)
        n += from->Length;
    if (values)
        n += (c = values->Count);

    array<T>^ to = gcnew array<T>(n);
    if (from)
        from->CopyTo(to, 0);
    if (values)
        values->CopyTo(to, to->Length-c);

    return to;
}


SvnPropertyCollection^ SvnBase::CreatePropertyDictionary(apr_hash_t* propHash, AprPool^ pool)
{
    if (!propHash)
        throw gcnew ArgumentNullException("propHash");
    else if (!pool)
        throw gcnew ArgumentNullException("pool");

    SvnPropertyCollection^ _properties = gcnew SvnPropertyCollection();

    for (apr_hash_index_t* hi = apr_hash_first(pool->Handle, propHash); hi; hi = apr_hash_next(hi))
    {
        const char* pKey;
        apr_ssize_t keyLen;
        const svn_string_t *propVal;

        apr_hash_this(hi, (const void**)&pKey, &keyLen, (void**)&propVal);

        _properties->Add(SvnPropertyValue::Create(pKey, propVal, nullptr));
    }

    return _properties;
}

apr_array_header_t *SvnBase::CreateChangeListsList(ICollection<String^>^ changelists, AprPool^ pool)
{
    if (!pool)
        throw gcnew ArgumentNullException("pool");
    else if (changelists && changelists->Count > 0)
        return AllocArray(changelists, pool);

    return nullptr;
}

apr_hash_t *SvnBase::CreateRevPropList(SvnRevisionPropertyCollection^ revProps, AprPool^ pool)
{
    if (!pool)
        throw gcnew ArgumentNullException("pool");
    else if (revProps && revProps->Count)
    {
        apr_hash_t* items = apr_hash_make(pool->Handle);

        for each (SvnPropertyValue^ value in revProps)
        {
            const char* key = pool->AllocString(value->Key);

            const svn_string_t* val = pool->AllocSvnString((array<Byte>^)value->RawValue);

            apr_hash_set(items, key, APR_HASH_KEY_STRING, val);
        }

        return items;
    }

    return nullptr;
}

String^ SvnBase::UriToString(Uri^ value)
{
    if (!value)
        return nullptr;

    if (value->IsAbsoluteUri)
        return value->GetComponents(
        UriComponents::SchemeAndServer |
        UriComponents::UserInfo |
        UriComponents::Path, UriFormat::UriEscaped);
    else
        return Uri::EscapeUriString(value->ToString()); // Escape back to valid uri form
}

String^ SvnBase::UriToCanonicalString(Uri^ value)
{
    if (!value)
        return nullptr;

    String^ name = SvnBase::UriToString(CanonicalizeUri(value));

    if (name && name->Length && (name[name->Length-1] == '/'))
        return name->TrimEnd('/'); // "svn://host:port" is canoncialized to "svn://host:port/" by the .Net Uri class
    else
        return name;
}

