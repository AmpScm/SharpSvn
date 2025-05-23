// Copyright 2007-2025 The SharpSvn Project
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

#define SHARPSVN_NO_ABORT
#include <libintl.h>

using namespace SharpSvn::Implementation;
using namespace SharpSvn;
using namespace System::Globalization;
using namespace System::Collections::Generic;
using namespace System::Threading;
using namespace System::Resources;

static char* _untranslatableMarker = "*!*!*!* - Untranslatable marker - *!*!*!*!";
static char* svn_gettext(const char* domain, const char* msgid, int category);

[module: SuppressMessage("Microsoft.Design", "CA1001:TypesThatOwnDisposableFieldsShouldBeDisposable", Scope="type", Target="SharpSvn.Implementation.SvnLocalizerData")];
[module: SuppressMessage("Microsoft.Design", "CA1031:DoNotCatchGeneralExceptionTypes", Scope="member", Target="SharpSvn.Implementation.SvnLocalizer.#gettext({modopt(System.Runtime.CompilerServices.IsConst),modopt(System.Runtime.CompilerServices.IsSignUnspecifiedByte)}System.SByte*)")];

namespace SharpSvn {
    namespace Implementation
    {
        ref class SvnLocalizerData sealed
        {
            initonly CultureInfo^ _ci;
            SvnLocalizerData^ _parent;
            initonly ResourceSet^ _res;
            initonly AprPool^ _pool;
            initonly apr_hash_t* _pStrHash;

        public:
            SvnLocalizerData(CultureInfo^ ci, ResourceManager^ mgr, AprPool^ pool)
            {
                _ci = ci;
                _pool = gcnew AprPool(pool);
                _res = mgr->GetResourceSet(ci, true, false);
                _pStrHash = apr_hash_make(pool->Handle);
            }

            void SetParent(SvnLocalizerData^ parent)
            {
                if (!parent)
                    throw gcnew ArgumentNullException("parent");
                else if (_parent)
                    throw gcnew InvalidOperationException();
        else if (parent == this)
            throw gcnew ArgumentOutOfRangeException();

                        _parent = parent;
            }

            char* GetText(const char* msgid, bool isWriter)
            {
                if (_res)
                {
                    bool skip = false;
                    char* r = (char*)apr_hash_get(_pStrHash, msgid, APR_HASH_KEY_STRING);

                    if (r)
                    {
                        if (r == _untranslatableMarker)
                            skip = true;
                        else
                            return r;
                    }

                    if (!skip)
                    {
                        if (!isWriter)
                            return nullptr; // Please upgrade lock

                        String^ value = _res->GetString(SvnBase::Utf8_PtrToString(msgid));

                        char* p = value ? const_cast<char*>(_pool->AllocString(value)) : nullptr;

                        apr_hash_set(_pStrHash, msgid, APR_HASH_KEY_STRING, p ? p : _untranslatableMarker);

                        return p;
                    }
                    else
                        return nullptr; // Must upgrade first
                }

                if (_parent)
                    return _parent->GetText(msgid, isWriter);

                return const_cast<char*>(msgid);
            }
        };

        ref class SvnLocalizer sealed
        {
            SvnLocalizer()
            {}
        internal:
            static ReaderWriterLock^ _rwLock;
            static ResourceManager^ _resManager;
            static AprPool^ _pool;
            static Dictionary<String^,SvnLocalizerData^>^ _langMap;


            static void Initialize();
            static char* gettext(const char *msgid);


        };
    }
};

static bool svn_gettext_enabled = false;

// Helper method to resolve X64 calling convention issues
#pragma unmanaged
static bool SetHandler()
{
    sharpsvn_dgettext_t* handler = svn_gettext;

    return (InterlockedExchangePointer((void**)&sharpsvn_dgettext, (void*)handler) != handler);
}
#pragma managed

void SvnClient::EnableSubversionLocalization()
{
    if (SetHandler())
    {
        SvnLocalizer::Initialize();

        svn_gettext_enabled = true;
    }
}

void SvnLocalizer::Initialize()
{
    _rwLock = gcnew ReaderWriterLock();
    _resManager = gcnew ResourceManager("SharpSvn.SvnText", SvnClient::typeid->Assembly);
    _langMap = gcnew Dictionary<String^,SvnLocalizerData^>(StringComparer::OrdinalIgnoreCase);
    _pool = gcnew AprPool();
}

char* SvnLocalizer::gettext(const char* msgid)
{
    CultureInfo^ ci = CultureInfo::CurrentUICulture;
    LockCookie lc;
    bool _isWriter = false;
    try
    {
        _rwLock->AcquireReaderLock(1000);
        try
        {
            SvnLocalizerData^ ld;

            if (!_langMap->TryGetValue(ci->Name, ld))
            {
                lc = _rwLock->UpgradeToWriterLock(10000); // Raise exception after ten seconds.. Use english then
                _isWriter = true;

                ld = gcnew SvnLocalizerData(ci, _resManager, _pool);
                _langMap->Add(ci->Name, ld);

                SvnLocalizerData^ ldI = ld;
                while (ci->Parent)
                {
                    ci = ci->Parent;

                    if (!ci->Name || ci == CultureInfo::InvariantCulture)
                        break;

                    SvnLocalizerData^ ldX;
                    if (_langMap->TryGetValue(ci->Name, ldX))
                    {
                        ldI->SetParent(ldX);
                        break;
                    }
                    else
                    {
                        ldX = gcnew SvnLocalizerData(ci, _resManager, _pool);
                        ldI->SetParent(ldX);
                        _langMap->Add(ci->Name, ldX);

                        ldI = ldX;
                    }
                }
            }

            char* txt = ld->GetText(msgid, _isWriter);

            if (!txt && !_isWriter)
            {
                lc = _rwLock->UpgradeToWriterLock(10000); // Raise exception after ten seconds.. Use english then

                return ld->GetText(msgid, true);
            }

            return txt;
        }
        finally
        {
            if (_isWriter)
                _rwLock->DowngradeFromWriterLock(lc);
            _rwLock->ReleaseReaderLock();
        }
    }
    catch(Exception^)
    {           // Will fail hard if we do not return something
        return const_cast<char*>(msgid);
    }
}


static char* svn_gettext(const char* domain, const char* msgid, int category)
{
    UNUSED_ALWAYS(domain);
    UNUSED_ALWAYS(category);

    if (!svn_gettext_enabled)
        return const_cast<char*>(msgid);

    char* value = SvnLocalizer::gettext(msgid);

    if (value)
        return value;
    else
        return const_cast<char*>(msgid);
}

