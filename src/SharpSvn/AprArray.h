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

#pragma once

using namespace System;

namespace SharpSvn {
	namespace Implementation {

		using System::Collections::Generic::ICollection;

		generic<typename T>
		where T : Object
		interface class IItemMarshaller
		{
			property int ItemSize
			{
				int get();
			}

			void Write(T value, void* ptr, AprPool^ pool);

			T Read(const void* ptr, AprPool^ pool);
		};

		generic<typename TManaged, typename TMarshaller>
		where TMarshaller : IItemMarshaller<TManaged>
		ref class AprArray : public SvnBase
		{
			AprPool^ _pool;
			apr_array_header_t *_handle;
			initonly IItemMarshaller<TManaged>^ _marshaller; // BH: replacing type IItemMarshaller<T>^ with R gives an internal compiler error in MSC 14.0
			initonly bool _readOnly;

		internal:
			AprArray(ICollection<TManaged>^ items, AprPool ^pool);
			AprArray(const apr_array_header_t* handle, AprPool ^pool);
			AprArray(System::Collections::IEnumerable^ items, AprPool ^pool);

		internal:
			property apr_array_header_t *Handle
			{
				[DebuggerStepThrough]
				apr_array_header_t *get()
				{
					_pool->Ensure();
					return _handle;
				}
			}

			~AprArray();

		public:
			property int Count
			{
				virtual int get()
				{
					return _handle->nelts;
				}
			}

			property TManaged default[int]
			{
				TManaged get(int index)
				{
					if (index < 0 || index >= Count)
						throw gcnew ArgumentOutOfRangeException("index", "Index out of range");

					const char* pData = Handle->elts;

					return _marshaller->Read(pData + _marshaller->ItemSize * index, _pool);
				}
			}

			array<TManaged>^ ToArray()
			{
				array<TManaged>^ items = gcnew array<TManaged>(Count);

				CopyTo(items, 0);

				return items;
			}

			void CopyTo(array<TManaged>^ item, int offset)
			{
				for (int i = 0; i < Count; i++)
					item[i+offset] = default[i];
			}

			property bool IsReadOnly
			{
				virtual bool get()
				{
					return _readOnly;
				}
			}
		};


		ref class AprCStrMarshaller sealed : public IItemMarshaller<String^>
		{
		public:
			AprCStrMarshaller()
			{}

			property int ItemSize
			{
				virtual int get()
				{
					return sizeof(char*);
				}
			}

			virtual void Write(String^ value, void* ptr, AprPool^ pool)
			{
				const char** ppStr = (const char**)ptr;
				*ppStr = pool->AllocString(value);
			}

			virtual String^ Read(const void* ptr, AprPool^ pool)
			{
				UNUSED_ALWAYS(pool);

				const char** ppcStr = (const char**)ptr;

				return SvnBase::Utf8_PtrToString(*ppcStr);
			}
		};

		ref class AprCStrPathMarshaller sealed : public IItemMarshaller<String^>
		{
		public:
			property int ItemSize
			{
				virtual int get()
				{
					return sizeof(char*);
				}
			}

			virtual void Write(String^ value, void* ptr, AprPool^ pool)
			{
				const char** ppStr = (const char**)ptr;

				*ppStr = pool->AllocPath(value);
			}

			virtual String^ Read(const void* ptr, AprPool^ pool)
			{
				UNUSED_ALWAYS(pool);
				const char** ppcStr = (const char**)ptr;

				return SvnBase::Utf8_PtrToString(svn_path_local_style(*ppcStr, pool->Handle));
			}
		};

		ref class AprCanonicalMarshaller sealed : public IItemMarshaller<String^>
		{
		public:
			property int ItemSize
			{
				virtual int get()
				{
					return sizeof(char*);
				}
			}

			virtual void Write(String^ value, void* ptr, AprPool^ pool)
			{
				const char** ppStr = (const char**)ptr;
				*ppStr = pool->AllocCanonical(value);
			}

			virtual String^ Read(const void* ptr, AprPool^ pool)
			{
				UNUSED_ALWAYS(pool);

				const char** ppcStr = (const char**)ptr;

				return SvnBase::Utf8_PtrToString(*ppcStr);
			}
		};

		ref class AprSvnRevNumMarshaller sealed : public IItemMarshaller<__int64>
		{
		public:
			property int ItemSize
			{
				virtual int get()
				{
					return sizeof(svn_revnum_t);
				}
			}

			virtual void Write(__int64 value, void* ptr, AprPool^ pool)
			{
				UNUSED_ALWAYS(pool);
				svn_revnum_t * pRev = (svn_revnum_t*)ptr;

				*pRev = (svn_revnum_t)value;
			}

			virtual __int64 Read(const void* ptr, AprPool^ pool)
			{
				UNUSED_ALWAYS(pool);

				return *(svn_revnum_t*)ptr;
			}
		};
	}
}
