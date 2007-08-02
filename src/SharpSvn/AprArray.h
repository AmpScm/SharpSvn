
#pragma once

using namespace System;

#include "SvnBase.h"
#include "AprPool.h"

namespace SharpSvn {
	namespace Apr {

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

			T Read(const void* ptr);
		};

		generic<typename T, typename R>
		where R : IItemMarshaller<T>
		ref class AprArray : public SvnHandleBase
		{
			AprPool^ _pool;
			apr_array_header_t *_handle;
			initonly IItemMarshaller<T>^ _marshaller; // BH: replacing type IItemMarshaller<T>^ with R gives an internal compiler error in MSC 14.0
			initonly bool _readOnly;

		internal:
			AprArray(ICollection<T>^ items, AprPool ^pool);
			AprArray(const apr_array_header_t* handle, AprPool ^pool);
			AprArray(System::Collections::IEnumerable^ items, AprPool ^pool);

		internal:
			property apr_array_header_t *Handle
			{
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

			property T default[int] 
			{
				T get(int index) 
				{
					if(index < 0 || index >= Count)
						throw gcnew IndexOutOfRangeException("Index out of range");

					const char* pData = Handle->elts;

					return _marshaller->Read(pData + _marshaller->ItemSize * index);
				}
			}

			array<T>^ ToArray()
			{
				array<T>^ items = gcnew array<T>(Count);

				CopyTo(items, 0);

				return items;
			}

			void CopyTo(array<T>^ item, int offset)
			{
				for(int i = 0; i < Count; i++)
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


		ref class AprCStrMarshaller sealed : public SvnBase, public IItemMarshaller<String^>
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

			virtual String^ Read(const void* ptr)
			{
				const char** ppcStr = (const char**)ptr;

				return Utf8_PtrToString(*ppcStr);
			}
		};

		ref class AprCStrPathMarshaller sealed : public SvnBase, public IItemMarshaller<String^>
		{
		public:
			AprCStrPathMarshaller()
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
				*ppStr = pool->AllocPath(value);
			}

			virtual String^ Read(const void* ptr)
			{
				const char** ppcStr = (const char**)ptr;

				return Utf8_PtrToString(*ppcStr);
			}
		};

		ref class AprSvnRevNumMarshaller sealed : public SvnBase, public IItemMarshaller<__int64>
		{
		public:
			AprSvnRevNumMarshaller()
			{}

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

			virtual __int64 Read(const void* ptr)
			{
				return *(svn_revnum_t*)ptr;
			}
		};
	}
}