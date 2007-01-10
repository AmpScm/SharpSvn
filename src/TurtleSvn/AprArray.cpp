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
