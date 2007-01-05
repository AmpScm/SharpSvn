
#pragma once

using namespace System;

#include "SvnBase.h"


namespace TurtleSvn {
	namespace Apr {

		public ref class AprPool : public SvnHandleBase, public System::IDisposable
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
			AprPool^ _parent;
			AprPoolTag^ _tag;
			apr_pool_t *_handle;

			AprPool(apr_pool_t *handle);
			!AprPool();
		public:
			~AprPool();
			

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

			void Ensure()
			{
				if(!_tag)
					throw gcnew ObjectDisposedException("AprPool");

				_tag->Ensure();
			}

		internal:
			apr_pool_t* Detach()
			{
				if(_handle)
				{
					_tag->Ensure();
					apr_pool_t *me = _handle;
					_handle = nullptr;
					_tag = nullptr;

					return me;
				}
				return nullptr;
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

			void Clear();

			void* Alloc(size_t size);
			void* AllocCleared(size_t size);
			const char* AllocString(String^ value);
		};
	}
}