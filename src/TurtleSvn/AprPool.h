
#pragma once

using namespace System;

#include "SvnBase.h"


namespace QQn {
	namespace Apr {

		public ref class AprPool : public QQn::Svn::SvnHandleBase, public System::IDisposable
		{
			ref class AprPoolTag : public IDisposable
			{
			private:
				bool _disposed;
				AprPoolTag^ _parent;

			public:
				AprPoolTag()
				{}

				AprPoolTag(AprPoolTag^ parent)
				{
					_parent = parent;
				}

			private:
				~AprPoolTag()
				{
					_disposed = true;
					_parent = nullptr;
				}

			public:
				void Ensure()
				{
					if(_disposed)
						throw gcnew ObjectDisposedException("AprPool");

					if(_parent)
						_parent->Ensure();
				}
			};

		private:
			AprPoolTag^ _tag;
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
					_tag->Ensure();

					return _handle;
				}
			}
		};
	}
}