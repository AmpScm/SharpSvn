// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#include "SvnAll.h"

#include "svn_dso.h"
#include "svn_utf.h"

using namespace SharpSvn::Implementation;
using namespace SharpSvn;

#pragma unmanaged
static bool _aprInitialized = false;
#pragma managed

static SvnBase::SvnBase()
{
	System::Reflection::AssemblyName^ name = gcnew System::Reflection::AssemblyName(SvnBase::typeid->Assembly->FullName);

	_clientName = name->Name + "/" + name->Version->ToString();
	_clientNames->Add(name->Name);

	EnsureLoaded();
}

void SvnBase::EnsureLoaded()
{
	if(!_aprInitialized)
	{
		_aprInitialized = true;

		if(apr_initialize()) // First
			throw gcnew InvalidOperationException();

		svn_dso_initialize(); // Before first pool

		apr_pool_t* pool = svn_pool_create(nullptr);
		svn_utf_initialize(pool);
		svn_fs_initialize(pool);

		if (getenv("SVN_ASP_DOT_NET_HACK"))
		{
			svn_wc_set_adm_dir("_svn", pool);
		}

		svn_ra_initialize(pool);

		_admDir = svn_wc_get_adm_dir(pool);
	}
}

SvnBase::SvnBase()
{
}

bool SvnBase::IsValidReposUri(Uri^ uri)
{
	if(!uri)
		throw gcnew ArgumentNullException("uri");
	
	if(!uri->IsAbsoluteUri)
		return false;
	else if(String::IsNullOrEmpty(uri->Scheme))
		return false;

	return true;
}


bool SvnBase::IsNotUri(String ^path)
{
	if(String::IsNullOrEmpty(path))
		return false;

	// Use the same stupid algorithm subversion uses to choose between Uri's and paths
	for(int i = 0; i < path->Length; i++)
	{
		wchar_t c = path[i];
		switch(c)
		{
		case '\\':
		case '/':
			return true;
		case ':':
			if(i < path->Length-2)
			{
				if((path[i+1] == '/') && (path[i+2] == '/'))
					return false;
			}
			return true;
		default:
			if(!wchar_t::IsLetter(c))
				return true;
		}
	}
	return true;
}

Uri^ SvnBase::CanonicalizeUri(Uri^ uri)
{
	if(!uri)
		throw gcnew ArgumentNullException("uri");

	String^ path = uri->AbsolutePath;
	if(path->Length > 0 && path[path->Length -1] == '/')
	{
		// Create a new uri with all / and \ characters at the end removed
		return gcnew Uri(uri, path->TrimEnd(System::IO::Path::DirectorySeparatorChar, System::IO::Path::AltDirectorySeparatorChar));
	}

	return uri;
}

String^ SvnBase::CanonicalizePath(String^ path)
{
	if(!path)
		throw gcnew ArgumentNullException("path");

	if(path->Length > 0 && ((path[path->Length-1] == System::IO::Path::DirectorySeparatorChar) || (path[path->Length-1] == System::IO::Path::AltDirectorySeparatorChar)))
		return path->TrimEnd(System::IO::Path::DirectorySeparatorChar, System::IO::Path::AltDirectorySeparatorChar);

	return path;
}

String^ SvnBase::Utf8_PtrToString(const char *ptr)
{
	if(!ptr)
		return nullptr;

	return Utf8_PtrToString(ptr, (int)::strlen(ptr));
}

String^ SvnBase::Utf8_PtrToString(const char *ptr, int length)
{
	if(!ptr || length < 0)
		return nullptr;

	return gcnew String(ptr, 0, length, System::Text::Encoding::UTF8);
}

String^ SvnBase::Utf8_PtrToString(const svn_string_t* str)
{
	if(!str || !str->data)
		return nullptr;

	return gcnew String(str->data, 0, str->len, System::Text::Encoding::UTF8);
}


array<Byte>^ SvnBase::PtrToByteArray(const char* ptr, int length)
{
	if(!ptr || length < 0)
		return nullptr;

	array<Byte>^ bytes = gcnew array<Byte>(length);

	pin_ptr<Byte> pBytes = &bytes[0];

	memcpy(pBytes, ptr, length);

	return bytes;
}

Object^ SvnBase::PtrToStringOrByteArray(const char* ptr, int length)
{
	if(!ptr || length < 0)
		return nullptr;
	else if(length == 0)
		return "";

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
	if(aprTime == 0)
		return DateTime::MinValue;
	else
	{
		__int64 aprTimeBase = DateTime(1970,1,1).ToBinary();

		return System::DateTime(aprTime*10 + aprTimeBase, DateTimeKind::Utc);
	}
}

apr_time_t SvnBase::AprTimeFromDateTime(DateTime time)
{
	__int64 aprTimeBase = DateTime(1970,1,1).ToBinary();

	if(time != DateTime::MinValue)
		return (time.ToBinary() - aprTimeBase) / 10;
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

		(*src)->path = pool->AllocString(value->TargetName);
		(*src)->revision = value->Revision->AllocSvnRevision(pool);
		(*src)->peg_revision = value->Revision->AllocSvnRevision(pool);
	}

	virtual SvnTarget^ Read(const void* ptr, AprPool^ pool)
	{
		UNUSED_ALWAYS(ptr);
		UNUSED_ALWAYS(pool);
		//const char** ppcStr = (const char**)ptr;

		return nullptr;
	}
};

apr_array_header_t *SvnBase::AllocCopyArray(ICollection<SvnTarget^>^ targets, AprPool^ pool)
{
	if(!targets)
		throw gcnew ArgumentNullException("targets");

	for each(SvnTarget^ s in targets)
	{
		if(!s)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targets");
	}
	AprArray<SvnTarget^, SvnCopyTargetMarshaller^>^ aprTargets = gcnew AprArray<SvnTarget^, SvnCopyTargetMarshaller^>(targets, pool);

	return aprTargets->Handle;
}

apr_array_header_t *SvnBase::AllocCopyArray(System::Collections::IEnumerable^ targets, AprPool^ pool)
{
	if(!targets)
		throw gcnew ArgumentNullException("targets");

	for each(SvnTarget^ s in targets)
	{
		if(!s)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "targets");
	}
	AprArray<SvnTarget^, SvnCopyTargetMarshaller^>^ aprTargets = gcnew AprArray<SvnTarget^, SvnCopyTargetMarshaller^>(targets, pool);

	return aprTargets->Handle;
}

SvnPropertyCollection^ SvnBase::CreatePropertyDictionary(apr_hash_t* propHash, AprPool^ pool)
{
	if(!propHash)
		throw gcnew ArgumentNullException("propHash");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	SvnPropertyCollection^ _properties = gcnew SvnPropertyCollection();

	for (apr_hash_index_t* hi = apr_hash_first(pool->Handle, propHash); hi; hi = apr_hash_next(hi))
	{
		const char* pKey;
		apr_ssize_t keyLen;
		const svn_string_t *propVal;

		apr_hash_this(hi, (const void**)&pKey, &keyLen, (void**)&propVal);

		String^ key = SvnBase::Utf8_PtrToString(pKey, (int)keyLen);
		Object^ value = SvnBase::PtrToStringOrByteArray(propVal->data, (int)propVal->len);

		String^ str = dynamic_cast<String^>(value);

		if(str)
		{
			if(key->StartsWith("svn:", StringComparison::Ordinal))
				str = str->Replace("\n", Environment::NewLine);

			_properties->Add(gcnew SvnPropertyValue(key, str));
		}
		else
			_properties->Add(gcnew SvnPropertyValue(key, safe_cast<array<Byte>^>(value)));
	}

	return _properties;
}

apr_array_header_t *SvnBase::CreateChangeListsList(ICollection<String^>^ changelists, AprPool^ pool)
{
	if(changelists && changelists->Count > 0)
		return AllocArray(changelists, pool);

	return nullptr;
}
