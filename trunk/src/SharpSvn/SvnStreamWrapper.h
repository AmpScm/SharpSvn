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

#include "SvnBase.h"
#include "AprBaton.h"


namespace SharpSvn {
	namespace Implementation {
		using System::IO::Stream;

		ref class SvnStreamWrapper : public IDisposable
		{
			initonly AprBaton<SvnStreamWrapper^>^ _streamBaton;
			initonly Stream^ _stream;
			svn_stream_t* _svnStream;
			AprPool^ _pool;


		public:
			SvnStreamWrapper(Stream^ stream, bool enableRead, bool enableWrite, AprPool^ pool)
			{
				if (!stream)
					throw gcnew ArgumentNullException("stream");
				else if (!enableRead && !enableWrite)
					throw gcnew ArgumentException("enableRead or enableWrite must be set to true");

				_stream = stream;
				_streamBaton = gcnew AprBaton<SvnStreamWrapper^>(this);
				_pool = pool;

				if (enableRead && !_stream->CanRead)
					throw gcnew InvalidOperationException("Can't enable reading on an unreadable stream");
				else if (enableWrite && !_stream->CanWrite)
					throw gcnew InvalidOperationException("Can't enable writing on an unwritable stream");

				Init(enableRead, enableWrite);
			}

			~SvnStreamWrapper();

			property svn_stream_t* Handle
			{
				svn_stream_t* get()
				{
					_pool->Ensure();
					return _svnStream;
				}
			}

		private:
			void Init(bool enableRead, bool enableWrite);

		internal:
			property Stream^ Stream
			{
				System::IO::Stream^ get()
				{
					return _stream;
				}
			}
		};

		ref class SvnWrappedStream sealed : System::IO::Stream
		{
			svn_stream_t* _stream;
			AprPool^ _pool;
		internal:
			SvnWrappedStream(svn_stream_t* stream, AprPool^ pool)
			{
				if (!stream)
					throw gcnew ArgumentNullException("stream");
				else if (!pool)
					throw gcnew ArgumentNullException("pool");

				_pool = pool;
				_stream = stream;
			}

		public:
			property bool CanRead
			{
				virtual bool get() override
				{
					return true;
				}
			}

			property bool CanSeek
			{
				virtual bool get() override
				{
					return false; // Only to the start?
				}
			}

			property bool CanWrite
			{
				virtual bool get() override
				{
					return false;
				}
			}

			property __int64 Length
			{
				virtual __int64 get() override
				{
					return -1; // Length unknown
				}
			}

			property __int64 Position
			{
				virtual __int64 get() override
				{
					return -1; // Position unknown
				}
				virtual void set(__int64 value) override
				{
					UNUSED_ALWAYS(value);
					throw gcnew InvalidOperationException();
				}
			}

			virtual void Flush() override
			{
			}

			virtual __int64 Seek(__int64 offset, System::IO::SeekOrigin origin) override
			{
				if (!_pool || !_pool->IsValid())
					throw gcnew InvalidOperationException();
				if (origin != System::IO::SeekOrigin::Begin)
					throw gcnew ArgumentOutOfRangeException("origin", origin, "Only SeekOrigin.Begin is supported");
				else if (offset != 0)
					throw gcnew ArgumentOutOfRangeException("offset", offset, "Only Seeking to 0 is supported");
				else if (!_pool->IsValid())
					throw gcnew InvalidOperationException();

				throw gcnew NotImplementedException("Seek to 0 will be available later");
			}

			virtual void SetLength(__int64 value) override
			{
				UNUSED_ALWAYS(value);
				throw gcnew InvalidOperationException();
			}

			virtual int Read(array<System::Byte>^ data, int offset, int length) override
			{
				if (!data)
					throw gcnew ArgumentNullException("data");
				else if (!_pool->IsValid())
					throw gcnew InvalidOperationException();
				else if (offset < 0 || offset >= data->Length)
					throw gcnew ArgumentOutOfRangeException("offset", offset, "Offset out of range");
				else if (length < 0 || length > data->Length || length + offset > data->Length)
					throw gcnew ArgumentOutOfRangeException("length", length, "Length out of range");

				pin_ptr<System::Byte> pData = &data[offset];

				apr_size_t len = length;
				svn_error_t* r = svn_stream_read(_stream, (char*)pData, &len);

				if (r)
					throw SvnException::Create(r);

				return (int)len;
			}

			virtual void Write(array<System::Byte>^ data, int offset, int length) override
			{
				UNUSED_ALWAYS(data);
				UNUSED_ALWAYS(offset);
				UNUSED_ALWAYS(length);
				throw gcnew InvalidOperationException();
			}
		};
	}
}
