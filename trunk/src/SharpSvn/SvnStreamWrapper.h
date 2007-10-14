// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

using namespace System;

#include "SvnBase.h"
#include "AprBaton.h"


namespace SharpSvn {
	namespace Apr {
		using System::IO::Stream;

		ref class SvnStreamWrapper : public IDisposable
		{
			initonly AprBaton<SvnStreamWrapper^>^ _streamBatton;
			initonly Stream^ _stream;
			svn_stream_t* _svnStream;
			AprPool^ _pool;


		public:
			SvnStreamWrapper(Stream^ stream, bool enableRead, bool enableWrite, AprPool^ pool)
			{
				if(!stream)
					throw gcnew ArgumentNullException("stream");
				else if(!enableRead && !enableWrite)
					throw gcnew ArgumentException("enableRead or enableWrite must be set to true");

				_stream = stream;
				_streamBatton = gcnew AprBaton<SvnStreamWrapper^>(this);
				_pool = pool;

				if(enableRead && !_stream->CanRead)
					throw gcnew InvalidOperationException("Can't enable reading on an unreadable stream");
				else if(enableWrite && !_stream->CanWrite)
					throw gcnew InvalidOperationException("Can't enable reading on an unwritable stream");

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
	}
}