
#pragma once

using namespace System;
using System::Collections::Generic::ICollection;
#include "SvnBase.h"


namespace SharpSvn {
	namespace Apr {

		ref class AprPool : public SvnHandleBase, public System::IDisposable
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

			internal:
				bool IsValid()
				{
					if(_disposed)
						return false;
					if(_parent)
						return _parent->IsValid();

					return true;					
				}

				
			};

		private:
			AprPool^ _parent;
			AprPoolTag^ _tag;
			apr_pool_t *_handle;
			bool _destroyPool;

			AprPool(apr_pool_t *handle, bool destroyPool);
			!AprPool();
		public:
			~AprPool();
			

		protected:
			void Destroy();

		public:
			AprPool(AprPool^ parentPool);
			AprPool();

			static AprPool^ Attach(apr_pool_t *handle, bool destroyPool);

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

					GC::SuppressFinalize(this);

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
			const char* AllocPath(String^ value);
			const svn_string_t* AllocSvnString(String^ value);
			const svn_string_t* AllocSvnString(array<char>^ value);

			static const char* AllocString(String^ value, apr_pool_t *pool);
			static const char* AllocPath(String^ path, apr_pool_t *pool);
			static void* Alloc(size_t size, apr_pool_t *pool);
			static void* AllocCleared(size_t size, apr_pool_t *pool);
			static const svn_string_t* AllocSvnString(String^ value, apr_pool_t *pool);
			static const svn_string_t* AllocSvnString(array<char>^ value, apr_pool_t *pool);
		};
	}
}