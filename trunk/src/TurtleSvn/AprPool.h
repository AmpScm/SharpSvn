
#pragma once

using namespace System;

#include "SvnBase.h"


namespace QQn {
	namespace Apr {

		public ref class AprPool : public QQn::Svn::SvnHandleBase, public System::IDisposable
		{
		private:
			apr_pool_t *_handle;

			AprPool(apr_pool_t *handle);
			~AprPool();
		public:
			!AprPool();

		protected:
			void Destroy();

		public:
			AprPool(AprPool^ parentPool);
			AprPool();

			property bool IsDisposed
			{
				bool get()
				{
					return !_handle;
				}
			}

		internal:
			property apr_pool_t* Handle
			{
				apr_pool_t* get()
				{
					if(!_handle)
						throw gcnew ObjectDisposedException("AprPool");

					return _handle;
				}
			}
		};
	}
}