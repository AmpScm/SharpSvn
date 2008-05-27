// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"

#include "SvnAll.h"

#include "svn_dso.h"
#include "svn_utf.h"

using System::Text::StringBuilder;
using System::IO::Path;
using namespace SharpSvn;

#pragma unmanaged
static bool _aprInitialized = false;
#pragma managed

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
	if (!_aprInitialized)
	{
		_aprInitialized = true;

		if (apr_initialize()) // First
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

		InstallAbortHandler();
	}
}

SvnBase::SvnBase()
{
}

AprPool^ SvnBase::SmallThreadPool::get()
{
	if(!_threadPool || !_threadPool->IsValid()) // Recreate if disposed for some reason
		_threadPool = gcnew AprPool();
	
	return _threadPool;
}

bool SvnBase::IsValidReposUri(Uri^ uri)
{
	if (!uri)
		throw gcnew ArgumentNullException("uri");

	if (!uri->IsAbsoluteUri)
		return false;
	else if(String::IsNullOrEmpty(uri->Scheme))
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
		default:
			if (!wchar_t::IsLetter(c))
				return true;
		}
	}
	return true;
}

Uri^ SvnBase::CanonicalizeUri(Uri^ uri)
{
	if (!uri)
		throw gcnew ArgumentNullException("uri");
	else if(!uri->IsAbsoluteUri)
		throw gcnew ArgumentException(SharpSvnStrings::UriIsNotAbsolute, "uri");

	String^ path = uri->AbsolutePath;
	if (path->Length > 0 && (path[path->Length -1] == '/' || path->IndexOf('\\') >= 0))
	{
		// Create a new uri with all / and \ characters at the end removed
		return gcnew Uri(uri, path->TrimEnd(System::IO::Path::DirectorySeparatorChar, System::IO::Path::AltDirectorySeparatorChar));
	}

	return uri;
}

Uri^ SvnBase::PathToUri(String^ path)
{
	if (!path)
		throw gcnew ArgumentNullException("path");

	StringBuilder^ sb = gcnew StringBuilder();
	Uri^ result;

	bool afterFirst = false;

	for each (String^ p in path->Split(Path::DirectorySeparatorChar, Path::AltDirectorySeparatorChar))
	{
		if (afterFirst)
			sb->Append((Char)'/');
		else
			afterFirst = true;

		sb->Append(Uri::EscapeDataString(p));
	}

	if(Uri::TryCreate(sb->ToString(), UriKind::Relative, result))
		return result;

	throw gcnew ArgumentOutOfRangeException();
}

/*String^ SvnBase::CanonicalizePath(String^ path)
{
	if (!path)
		throw gcnew ArgumentNullException("path");

	if (path->Length > 0 && ((path[path->Length-1] == System::IO::Path::DirectorySeparatorChar) || (path[path->Length-1] == System::IO::Path::AltDirectorySeparatorChar)))
		return path->TrimEnd(System::IO::Path::DirectorySeparatorChar, System::IO::Path::AltDirectorySeparatorChar);

	return path;
}*/

String^ SvnBase::Utf8_PtrToString(const char *ptr)
{
	if (!ptr)
		return nullptr;

	return Utf8_PtrToString(ptr, (int)::strlen(ptr));
}

String^ SvnBase::Utf8_PtrToString(const char *ptr, int length)
{
	if (!ptr || length < 0)
		return nullptr;

	return gcnew String(ptr, 0, length, System::Text::Encoding::UTF8);
}

Uri^ SvnBase::Utf8_PtrToUri(const char *ptr, SvnNodeKind nodeKind)
{
	if (!ptr)
		return nullptr;

	String^ url = Utf8_PtrToString(ptr);

	if (!url)
		return nullptr;
	else if(nodeKind == SvnNodeKind::Directory && !url->EndsWith("/", StringComparison::Ordinal))
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

	pin_ptr<Byte> pBytes = &bytes[0];

	memcpy(pBytes, ptr, length);

	return bytes;
}

Object^ SvnBase::PtrToStringOrByteArray(const char* ptr, int length)
{
	if (!ptr || length < 0)
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
	if (aprTime == 0)
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

	if (time != DateTime::MinValue)
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

		(*src)->path = pool->AllocCanonical(value->SvnTargetName);
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

SvnPropertyCollection^ SvnBase::CreatePropertyDictionary(apr_hash_t* propHash, AprPool^ pool)
{
	if (!propHash)
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
	else if(revProps && revProps->Count)
	{
		apr_hash_t* items = apr_hash_make(pool->Handle);
		
		for each(SvnPropertyValue^ value in revProps)
		{
			const char* key = pool->AllocString(value->Key);

			const svn_string_t* val = pool->AllocSvnString((array<Byte>^)value->RawValue);

			apr_hash_set(items, key, APR_HASH_KEY_STRING, val);
		}

		return items;
	}

	return nullptr;
}