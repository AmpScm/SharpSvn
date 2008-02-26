// $Id: SvnBase.cpp 346 2008-02-22 12:48:18Z bhuijben $
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#include "SvnAll.h"

#include "svn_dso.h"
#include "svn_utf.h"
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

namespace SharpSvn {
	namespace Implementation
	{
		ref class SvnLocalizerData sealed
		{
			CultureInfo^ _ci;
			SvnLocalizerData^ _parent;
			ResourceSet^ _res;
			AprPool^ _pool;
			apr_hash_t* _ptrHash;
			apr_hash_t* _pStrHash;
			

		public:
			SvnLocalizerData(CultureInfo^ ci, ResourceManager^ mgr, AprPool^ pool)
			{
				_ci = ci;
				_pool = gcnew AprPool(pool);
				_res = mgr->GetResourceSet(ci, true, false);
				_ptrHash = apr_hash_make(pool->Handle);
				_pStrHash = apr_hash_make(pool->Handle);
			}

			void SetParent(SvnLocalizerData^ parent)
			{
				if(!parent)
					throw gcnew ArgumentNullException("parent");
				else if(_parent)
					throw gcnew InvalidOperationException();

				_parent = parent;
			}

			char* GetText(const char* msgid, bool isWriter)
			{
				if(_res)
				{
					bool skip = false;
					char* r = (char*)apr_hash_get(_ptrHash, &msgid, sizeof(char*));

					if(r)
					{
						if(r == _untranslatableMarker)
							skip = true;
						else
							return r;
					}
					
					if(!skip)
					{
						r = (char*)apr_hash_get(_pStrHash, msgid, APR_HASH_KEY_STRING);

						if(r)
						{
							if(r == _untranslatableMarker)
								skip = true;
							else
								return r;
						}
					}

					if(!skip)
					{
						if(!isWriter)
							return false; // Please upgrade lock

						String^ value = _res->GetString(SvnBase::Utf8_PtrToString(msgid));

						char* p = value ? const_cast<char*>(_pool->AllocString(value)) : nullptr;

						apr_hash_set(_ptrHash, &msgid, sizeof(char*), p ? p : _untranslatableMarker); // Skip next time
						apr_hash_set(_pStrHash, msgid, APR_HASH_KEY_STRING, p ? p : _untranslatableMarker);

						return p;
					}
					else
						return nullptr; // Must upgrade first
				}
				
				if(_parent)
					return _parent->GetText(msgid, isWriter);

				return const_cast<char*>(msgid);
			}
		};

		ref class SvnLocalizer sealed
		{
		internal:
			static ReaderWriterLock^ _rwLock;
			static ResourceManager^ _resManager;
			static AprPool^ _pool;
			static apr_hash_t *_resHash;
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
	if(SetHandler())
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

			if(!_langMap->TryGetValue(ci->Name, ld))
			{
				lc = _rwLock->UpgradeToWriterLock(10000); // Raise exception after ten seconds.. Use english then
				_isWriter = true;

				ld = gcnew SvnLocalizerData(ci, _resManager, _pool);
				_langMap->Add(ci->Name, ld);

				SvnLocalizerData^ ldI = ld;
				while(ci->Parent)
				{
					ci = ci->Parent;

					if(!ci->Name || ci == CultureInfo::InvariantCulture)
						break;

					SvnLocalizerData^ ldX;
					if(_langMap->TryGetValue(ci->Name, ldX))
					{
						ldI->SetParent(ld);
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

			if(!txt && !_isWriter)
			{
				lc = _rwLock->UpgradeToWriterLock(10000); // Raise exception after ten seconds.. Use english then

				return ld->GetText(msgid, true);
			}

			return txt;
		}
		finally
		{
			if(_isWriter)
				_rwLock->DowngradeFromWriterLock(lc);
			_rwLock->ReleaseReaderLock();
		}
	}
	catch(Exception^)
	{	// Will fail hard if we do not return something
		return const_cast<char*>(msgid);
	}
}


static char* svn_gettext(const char* domain, const char* msgid, int category)
{
	UNUSED_ALWAYS(domain);
	UNUSED_ALWAYS(category);

	if(!svn_gettext_enabled)
		return const_cast<char*>(msgid);

	return SvnLocalizer::gettext(msgid);
}

