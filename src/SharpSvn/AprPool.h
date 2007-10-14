// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

using namespace System;
using System::Collections::Generic::ICollection;
#include "SvnBase.h"
#include <svn_pools.h>


namespace SharpSvn {
	namespace Apr {

		[SecurityPermission(SecurityAction::InheritanceDemand, UnmanagedCode=true)]
		[SecurityPermission(SecurityAction::LinkDemand, UnmanagedCode=true)]
		ref class AprPool sealed : public SvnHandleBase, public System::IDisposable
		{
			static const __int64 StandardMemoryPressure = 512*1024;

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
				[System::Diagnostics::DebuggerStepThroughAttribute()]
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
			initonly bool _destroyPool;

			!AprPool();
		public:
			~AprPool();


		private:
			void Destroy();

		public:
			AprPool(AprPool^ parentPool);
			AprPool();
			AprPool(apr_pool_t *attachHandle, bool destroyPool);

			[System::Diagnostics::DebuggerStepThroughAttribute()]
			void Ensure()
			{
				if(!_tag)
					throw gcnew ObjectDisposedException("AprPool");

				_tag->Ensure();
			}

		internal:
			property apr_pool_t* Handle
			{
				[System::Diagnostics::DebuggerStepThroughAttribute()]
				apr_pool_t* get()
				{
					_tag->Ensure();

					return _handle;
				}
			}

			[System::Diagnostics::DebuggerStepThroughAttribute()]
			void Clear();

			[System::Diagnostics::DebuggerStepThroughAttribute()]
			void* Alloc(size_t size);
			[System::Diagnostics::DebuggerStepThroughAttribute()]
			void* AllocCleared(size_t size);
			[System::Diagnostics::DebuggerStepThroughAttribute()]
			const char* AllocString(String^ value);
			[System::Diagnostics::DebuggerStepThroughAttribute()]
			const char* AllocUnixString(String^ value);
			[System::Diagnostics::DebuggerStepThroughAttribute()]
			const char* AllocPath(String^ value);
			[System::Diagnostics::DebuggerStepThroughAttribute()]
			const char* AllocCanonical(String^ value);
			[System::Diagnostics::DebuggerStepThroughAttribute()]
			const svn_string_t* AllocSvnString(String^ value);
			[System::Diagnostics::DebuggerStepThroughAttribute()]
			const svn_string_t* AllocUnixSvnString(String^ value);
			[System::Diagnostics::DebuggerStepThroughAttribute()]
			const svn_string_t* AllocSvnString(array<char>^ value);
		};
	}
}