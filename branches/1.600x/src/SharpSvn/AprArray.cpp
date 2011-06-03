// $Id$
//
// Copyright 2007-2008 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "stdafx.h"
#include "SvnAll.h"

[module: SuppressMessage("Microsoft.Performance", "CA1812:AvoidUninstantiatedInternalClasses", Scope="type", Target="SharpSvn.Implementation.AprCStrPathMarshaller")];
[module: SuppressMessage("Microsoft.Performance", "CA1812:AvoidUninstantiatedInternalClasses", Scope="type", Target="SharpSvn.Implementation.AprCanonicalMarshaller")];
[module: SuppressMessage("Microsoft.Performance", "CA1812:AvoidUninstantiatedInternalClasses", Scope="type", Target="SharpSvn.Implementation.AprSvnRevNumMarshaller")];
[module: SuppressMessage("Microsoft.Performance", "CA1812:AvoidUninstantiatedInternalClasses", Scope="type", Target="SharpSvn.Implementation.AprCStrMarshaller")];
[module: SuppressMessage("Microsoft.Performance", "CA1812:AvoidUninstantiatedInternalClasses", Scope="type", Target="SharpSvn.Implementation.SvnMergeRangeMarshaller")];

using namespace SharpSvn::Implementation;

generic<typename T, typename R>
AprArray<T,R>::AprArray(ICollection<T>^ items, AprPool ^pool)
{
	if (!items)
		throw gcnew ArgumentNullException("items");
	else if (!pool)
		throw gcnew ArgumentNullException("pool");

	_marshaller = Activator::CreateInstance<R>();
	_pool = pool;
	_handle = apr_array_make(pool->Handle, items->Count, _marshaller->ItemSize);

	for each (T t in items)
	{
		void* ptr = apr_array_push(_handle);

		_marshaller->Write(t, ptr, pool);
	}
}

generic<typename T, typename R>
AprArray<T,R>::AprArray(System::Collections::IEnumerable^ items, AprPool ^pool)
{
	if (!items)
		throw gcnew ArgumentNullException("items");
	else if (!pool)
		throw gcnew ArgumentNullException("pool");

	int nItems = 0;
	for each (T t in items)
	{
		if (t == nullptr)
			throw gcnew ArgumentException(SharpSvn::SharpSvnStrings::ItemInListIsNull, "items");

		nItems++;
	}

	_marshaller = Activator::CreateInstance<R>();
	_pool = pool;
	_handle = apr_array_make(pool->Handle, nItems, _marshaller->ItemSize);

	for each (T t in items)
	{
		void* ptr = apr_array_push(_handle);

		_marshaller->Write(t, ptr, pool);
	}
}


generic<typename T, typename R>
AprArray<T,R>::AprArray(const apr_array_header_t* handle, AprPool ^pool)
{
	if (!handle)
		throw gcnew ArgumentNullException("handle");
	else if (!pool)
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
	if (!strings)
		throw gcnew ArgumentNullException("strings");
	else if (!pool)
		throw gcnew ArgumentNullException("pool");

	for each (String^ s in strings)
	{
		if (!s)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "strings");
	}
	AprArray<String^, AprCStrMarshaller^>^ aprStrings = gcnew AprArray<String^, AprCStrMarshaller^>(strings, pool);

	return aprStrings->Handle;
}

apr_array_header_t *SvnBase::AllocCanonicalArray(ICollection<String^>^ paths, AprPool^ pool)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");
	else if (!pool)
		throw gcnew ArgumentNullException("pool");

	for each (String^ s in paths)
	{
		if (!s)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "paths");
	}
	AprArray<String^, AprCanonicalMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCanonicalMarshaller^>(paths, pool);

	return aprPaths->Handle;
}


apr_array_header_t *SvnBase::AllocPathArray(ICollection<String^>^ paths, AprPool^ pool)
{
	if (!paths)
		throw gcnew ArgumentNullException("paths");
	else if (!pool)
		throw gcnew ArgumentNullException("pool");

	for each (String^ s in paths)
	{
		if (!s)
			throw gcnew ArgumentException(SharpSvnStrings::ItemInListIsNull, "paths");
	}
	AprArray<String^, AprCStrPathMarshaller^>^ aprPaths = gcnew AprArray<String^, AprCStrPathMarshaller^>(paths, pool);

	return aprPaths->Handle;
}

