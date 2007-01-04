
#pragma once

using namespace System;

#include "SvnBase.h"
#include "AprPool.h"

namespace QQn {
	namespace Apr {

		using System::Collections::Generic::IList;

		public ref class AprMemoryItem abstract
		{
		internal:
			AprMemoryItem()
			{
			}

			property int ItemSize
			{
				virtual int get() abstract;
			}
		};

		generic<typename T>
		where T : AprMemoryItem
		public ref class AprArray : QQn::Svn::SvnHandleBase
		{
			AprPool^ _pool;
			apr_array_header_t *_handle;

		internal:
			AprArray(IList<T>^ items, AprPool ^pool);

		internal:
			property apr_array_header_t *Handle
			{
				apr_array_header_t *get();
			}

			!AprArray();
			~AprArray();
		};
	}
}