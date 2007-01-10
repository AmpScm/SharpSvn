#include "stdafx.h"
#include "SvnBase.h"

#include "AprPool.h"
#include "svn_dso.h"
#include "svn_utf.h"

#include <apr_general.h>

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

String^ SvnBase::Utf8_PtrToString(const char *ptr)
{
	if(!ptr)
		return nullptr;

	return Utf8_PtrToString(ptr, ::strlen(ptr));
}

String^ SvnBase::Utf8_PtrToString(const char *ptr, int length)
{
	if(!ptr)
		return nullptr;

	return gcnew String(ptr, 0, ::strlen(ptr), System::Text::Encoding::UTF8);
}

const char* SvnBase::Utf8_StringToPtr(String ^value, AprPool^ pool)
{
	if(!value)
		throw gcnew ArgumentNullException("value");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	return pool->AllocString(value);
}

DateTime SvnBase::DateTimeFromAprTime(apr_time_t aprTime)
{
	__int64 aprTimeBase = DateTime(1970,1,1).ToBinary();

	return System::DateTime(aprTime*10 + aprTimeBase);
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

const char* SvnHandleBase::Utf8_StringToPtr(String ^value, AprPool^ pool)
{
	return SvnBase::Utf8_StringToPtr(value, pool);
}
