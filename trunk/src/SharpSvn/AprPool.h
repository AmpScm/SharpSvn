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
	namespace Implementation {

		[SecurityPermission(SecurityAction::InheritanceDemand, UnmanagedCode=true)]
		[SecurityPermission(SecurityAction::LinkDemand, UnmanagedCode=true)]
		ref class AprPool sealed : public SvnHandleBase, public System::IDisposable
		{
			static const int StandardMemoryPressure = 512*1024;

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
					if (_disposed)
						throw gcnew ObjectDisposedException("AprPool");

					if (_parent)
						_parent->Ensure();
				}

			internal:
				bool IsValid()
				{
					if (_disposed)
						return false;
					if (_parent)
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
			/// <summary>Creates a childpool within the specified parent pool</summary>
			AprPool(AprPool^ parentPool);
			/// <summary>Creates a new root pool</summary>
			AprPool();
			/// <summary>Attaches to the specified pool</summary>
			AprPool(apr_pool_t *handle, bool destroyPool);

			[System::Diagnostics::DebuggerStepThroughAttribute()]
			void Ensure()
			{
				if (!_tag)
					throw gcnew ObjectDisposedException("AprPool");

				_tag->Ensure();
			}

			bool IsValid()
			{
				return _tag && _tag->IsValid();
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
			const char* AllocCanonical(Uri^ value);
			[System::Diagnostics::DebuggerStepThroughAttribute()]
			const svn_string_t* AllocSvnString(String^ value);
			[System::Diagnostics::DebuggerStepThroughAttribute()]
			const svn_string_t* AllocUnixSvnString(String^ value);
			[System::Diagnostics::DebuggerStepThroughAttribute()]
			const svn_string_t* AllocSvnString(array<Byte>^ value);
		};


		ref class AprStreamFile sealed : IDisposable
		{
		private:
			AprPool^ _pool;
			apr_file_t* _extHandle;
			apr_file_t* _intHandle;
			initonly System::IO::Stream^ _stream;
			initonly System::Threading::Thread^ _thread;

		private:
			~AprStreamFile()
			{
				if (_extHandle)
				{
					if (!apr_file_close(_extHandle))
					{
						if (!_thread->Join(60000)) // 60 sec Timeout should never happen
							throw gcnew InvalidOperationException(SharpSvnStrings::IOThreadBlocked);
					}

					_extHandle = nullptr;
				}
			}

			void Runner()
			{
				array<Byte>^ buffer = gcnew array<Byte>(2048);
				pin_ptr<Byte> pBuffer = &buffer[0];

				apr_size_t nRead = buffer->Length;
				apr_status_t r;

				while (APR_EOF != (r = apr_file_read(_intHandle, pBuffer, &nRead)))
				{
					if (r == APR_SUCCESS)
						_stream->Write(buffer, 0, (int)nRead);
					else if(!APR_STATUS_IS_EAGAIN(r) && !APR_STATUS_IS_EINTR(r))
						break; // Most errors are fatal

					nRead = buffer->Length;
				}
			}

		public:
			AprStreamFile(System::IO::Stream^ stream, AprPool^ pool)
			{
				if (!stream)
					throw gcnew ArgumentNullException("stream");
				else if(!pool)
					throw gcnew ArgumentNullException("pool");

				_stream = stream;
				_thread = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(this, &AprStreamFile::Runner));
				_pool = pool;
			}

		public:

			apr_file_t* CreateHandle()
			{
				if (!_extHandle)
				{
					apr_file_t* extHandle = nullptr;
					apr_file_t* intHandle = nullptr;
					if (apr_file_pipe_create(&intHandle, &extHandle, _pool->Handle) || !extHandle || !intHandle)
					{
						_extHandle = nullptr;
						_intHandle = nullptr;
						throw gcnew InvalidOperationException();
					}

					_extHandle = extHandle;
					_intHandle = intHandle;

					_thread->Start();
				}
				return _extHandle;
			}
		};
	}
}

/* Loosly from apr 1.3/trunk: This function is not available in apr 1.2 as used 2008-02-26 */
void sharpsvn_apr_hash_clear(apr_hash_t *ht);
