#include "stdafx.h"

#include "AprArray.h"

using namespace QQn::Apr;

generic<typename T>
AprArray<T>::AprArray(IList<T>^ items, AprPool ^pool)
{
	//apr_array_header_t
}

generic<typename T>
apr_array_header_t *AprArray<T>::Handle::get()
{
	_pool->Ensure();
	return _handle;
}

generic<typename T>
AprArray<T>::!AprArray()
{
}

generic<typename T>
AprArray<T>::~AprArray()
{
}