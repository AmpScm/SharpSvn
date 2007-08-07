#include "stdafx.h"

#include "AprArray.h"

using namespace SharpSvn::Apr;

generic<typename T, typename R>
AprArray<T,R>::AprArray(ICollection<T>^ items, AprPool ^pool)
{
	if(!items)
		throw gcnew ArgumentNullException("items");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	_marshaller = Activator::CreateInstance<R>();
	_pool = pool;
	_handle = apr_array_make(pool->Handle, items->Count, _marshaller->ItemSize);

	for each(T t in items)
	{
		void* ptr = apr_array_push(_handle);

		_marshaller->Write(t, ptr, pool);
	}
}

generic<typename T, typename R>
AprArray<T,R>::AprArray(System::Collections::IEnumerable^ items, AprPool ^pool)
{
	if(!items)
		throw gcnew ArgumentNullException("items");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	int nItems = 0;
	for each(T t in items)
	{
		if(!t)
			throw gcnew ArgumentException("item is null", "items");

		nItems++;
	}

	_marshaller = Activator::CreateInstance<R>();
	_pool = pool;
	_handle = apr_array_make(pool->Handle, nItems, _marshaller->ItemSize);

	for each(T t in items)
	{
		void* ptr = apr_array_push(_handle);

		_marshaller->Write(t, ptr, pool);
	}
}


generic<typename T, typename R>
AprArray<T,R>::AprArray(const apr_array_header_t* handle, AprPool ^pool)
{
	if(!handle)
		throw gcnew ArgumentNullException("handle");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	_marshaller = Activator::CreateInstance<R>();
	_handle = const_cast<apr_array_header_t*>(handle);
	_pool = pool;
	_readOnly = true;
}

generic<typename T, typename R>
AprArray<T,R>::~AprArray()
{
	_handle = nullptr;
}

apr_array_header_t *SvnHandleBase::AllocArray(ICollection<String^>^ strings, AprPool^ pool)
{
	if(!strings)
		throw gcnew ArgumentNullException("strings");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	for each(String^ s in strings)
	{
		if(!s)
			throw gcnew ArgumentException("String in strings is null", "strings");
	}
	AprArray<String^, AprCStrMarshaller^>^ aprStrings = gcnew AprArray<String^, AprCStrMarshaller^>(strings, pool);

	return aprStrings->Handle;
}

apr_array_header_t *SvnHandleBase::AllocPathArray(ICollection<String^>^ paths, AprPool^ pool)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	for each(String^ s in paths)
	{
		if(!s)
			throw gcnew ArgumentException("String in paths is null", "paths");
	}
	AprArray<String^, AprCStrPathMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCStrPathMarshaller^>(paths, pool);

	return aprPaths->Handle;
}

