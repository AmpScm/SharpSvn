#include "stdafx.h"
#include "SvnBase.h"

#include "AprPool.h"
#include "svn_dso.h"
#include "svn_utf.h"

#include <apr_general.h>

#include "AprArray.h"
#include "SvnTarget.h"


using namespace SharpSvn::Apr;
using namespace SharpSvn;

static bool _aprInitialized = false;
static int _myLen = 0;

static SvnBase::SvnBase()
{
	EnsureLoaded();
}

void SvnBase::EnsureLoaded()
{
	if(!_aprInitialized)
	{
		_aprInitialized = true;

		apr_initialize(); // First
		svn_dso_initialize(); // Before first pool

		apr_pool_t* pool;
		apr_pool_create(&pool, NULL);
		svn_utf_initialize(pool);

		svn_ra_initialize(pool);
	}
}

SvnBase::SvnBase()
{
}

bool SvnBase::IsNotUri(String ^path)
{
	if(!path)
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

String^ SvnBase::Utf8_PtrToString(const char *ptr)
{
	if(!ptr)
		return nullptr;

	return Utf8_PtrToString(ptr, ::strlen(ptr));
}

String^ SvnBase::Utf8_PtrToString(const char *ptr, int length)
{
	if(!ptr || length < 0)
		return nullptr;

	return gcnew String(ptr, 0, ::strlen(ptr), System::Text::Encoding::UTF8);
}

array<char>^ SvnBase::PtrToByteArray(const char* ptr, int length)
{
	if(!ptr || length < 0)
		return nullptr;

	array<char>^ bytes = gcnew array<char>(length);

	pin_ptr<char> pBytes = &bytes[0];

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
}

DateTime SvnBase::DateTimeFromAprTime(apr_time_t aprTime)
{
	__int64 aprTimeBase = DateTime(1970,1,1).ToBinary();

	return System::DateTime(aprTime*10 + aprTimeBase, DateTimeKind::Utc);
}

apr_time_t SvnBase::AprTimeFromDateTime(DateTime time)
{
	__int64 aprTimeBase = DateTime(1970,1,1).ToBinary();
	return (time.ToBinary() - aprTimeBase) / 10;
}

static SvnHandleBase::SvnHandleBase()
{
	SvnBase::EnsureLoaded();
}

SvnHandleBase::SvnHandleBase()
{
}

String^ SvnHandleBase::Utf8_PtrToString(const char *ptr)
{
	return SvnBase::Utf8_PtrToString(ptr);
}

String^ SvnHandleBase::Utf8_PtrToString(const char *ptr, int length)
{
	return SvnBase::Utf8_PtrToString(ptr, length);
}

bool SvnHandleBase::IsNotUri(String ^path)
{
	return SvnBase::IsNotUri(path);
}

array<char>^ SvnHandleBase::PtrToByteArray(const char* ptr, int length)
{
	return SvnBase::PtrToByteArray(ptr, length);
}

ref class SvnCopyTargetMarshaller : public SvnBase, public IItemMarshaller<SvnTarget^>
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
		svn_client_copy_source_t **src = (svn_client_copy_source_t**)&ptr;
		*src = (svn_client_copy_source_t *)pool->AllocCleared(sizeof(svn_client_copy_source_t));
		
		(*src)->path = pool->AllocString(value->TargetName);
		(*src)->revision = value->Revision->AllocSvnRevision(pool);
	}

	virtual SvnTarget^ Read(const void* ptr)
	{
		UNUSED_ALWAYS(ptr);
		//const char** ppcStr = (const char**)ptr;

		return nullptr;
	}
};

apr_array_header_t *SvnHandleBase::AllocCopyArray(ICollection<SvnTarget^>^ targets, AprPool^ pool)
{
	if(!targets)
		throw gcnew ArgumentNullException("targets");

	for each(SvnTarget^ s in targets)
	{
		if(!s)
			throw gcnew ArgumentException("SvnTarget in targets is null", "targets");
	}
	AprArray<SvnTarget^, SvnCopyTargetMarshaller^>^ aprTargets = gcnew AprArray<SvnTarget^, SvnCopyTargetMarshaller^>(targets, pool);

	return aprTargets->Handle;
}

apr_array_header_t *SvnHandleBase::AllocCopyArray(System::Collections::IEnumerable^ targets, AprPool^ pool)
{
	if(!targets)
		throw gcnew ArgumentNullException("targets");

	for each(SvnTarget^ s in targets)
	{
		if(!s)
			throw gcnew ArgumentException("SvnTarget in targets is null", "targets");
	}
	AprArray<SvnTarget^, SvnCopyTargetMarshaller^>^ aprTargets = gcnew AprArray<SvnTarget^, SvnCopyTargetMarshaller^>(targets, pool);

	return aprTargets->Handle;
}
