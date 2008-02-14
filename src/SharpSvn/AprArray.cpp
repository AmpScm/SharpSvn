// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#include "stdafx.h"
#include "SvnAll.h"

[module: SuppressMessage("Microsoft.Performance", "CA1812:AvoidUninstantiatedInternalClasses", Scope="type", Target="SharpSvn.Implementation.AprCStrPathMarshaller")];
[module: SuppressMessage("Microsoft.Performance", "CA1812:AvoidUninstantiatedInternalClasses", Scope="type", Target="SharpSvn.Implementation.AprCanonicalMarshaller")];
[module: SuppressMessage("Microsoft.Performance", "CA1812:AvoidUninstantiatedInternalClasses", Scope="type", Target="SharpSvn.Implementation.AprSvnRevNumMarshaller")];
[module: SuppressMessage("Microsoft.Performance", "CA1812:AvoidUninstantiatedInternalClasses", Scope="type", Target="SharpSvn.Implementation.AprCStrMarshaller")];

using namespace SharpSvn::Implementation;

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
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "items");

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

apr_array_header_t *SvnBase::AllocArray(ICollection<String^>^ strings, AprPool^ pool)
{
	if(!strings)
		throw gcnew ArgumentNullException("strings");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	for each(String^ s in strings)
	{
		if(!s)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "strings");
	}
	AprArray<String^, AprCStrMarshaller^>^ aprStrings = gcnew AprArray<String^, AprCStrMarshaller^>(strings, pool);

	return aprStrings->Handle;
}

apr_array_header_t *SvnBase::AllocCanonicalArray(ICollection<String^>^ paths, AprPool^ pool)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	for each(String^ s in paths)
	{
		if(!s)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "paths");
	}
	AprArray<String^, AprCanonicalMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCanonicalMarshaller^>(paths, pool);

	return aprPaths->Handle;
}


apr_array_header_t *SvnBase::AllocPathArray(ICollection<String^>^ paths, AprPool^ pool)
{
	if(!paths)
		throw gcnew ArgumentNullException("paths");
	else if(!pool)
		throw gcnew ArgumentNullException("pool");

	for each(String^ s in paths)
	{
		if(!s)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "paths");
	}
	AprArray<String^, AprCStrPathMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCStrPathMarshaller^>(paths, pool);

	return aprPaths->Handle;
}

