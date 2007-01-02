#include "stdafx.h"
#include "SvnBase.h"

#include <apr_general.h>

using namespace QQn::Svn;

static bool _aprInitialized = false;

static SvnBase::SvnBase()
{
	if(!_aprInitialized)
	{
		_aprInitialized = true;

		apr_initialize();
	}
}

void SvnBase::EnsureLoaded()
{
	GC::KeepAlive(SvnBase::typeid);
}

SvnBase::SvnBase()
{
}

String^ SvnBase::PtrToStringUtf8(const char *ptr)
{
	if(!ptr)
		return nullptr;

	return System::Runtime::InteropServices::Marshal::PtrToStringAnsi((IntPtr)const_cast<char*>(ptr));
}

String^ SvnBase::PtrToStringUtf8(const char *ptr, int length)
{
	if(!ptr)
		return nullptr;

	return System::Runtime::InteropServices::Marshal::PtrToStringAnsi((IntPtr)const_cast<char*>(ptr), length);
}

static SvnHandleBase::SvnHandleBase()
{
	SvnBase::EnsureLoaded();
}

SvnHandleBase::SvnHandleBase()
{
}

String^ SvnHandleBase::PtrToStringUtf8(const char *ptr)
{
	return SvnBase::PtrToStringUtf8(ptr);
}

String^ SvnHandleBase::PtrToStringUtf8(const char *ptr, int length)
{
	return SvnBase::PtrToStringUtf8(ptr, length);
}

