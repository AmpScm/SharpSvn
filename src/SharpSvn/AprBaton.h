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

namespace SharpSvn {
	namespace Implementation
	{
		using System::Runtime::InteropServices::GCHandle;

		generic<typename T>
		ref class AprBaton : public IDisposable
		{
			GCHandle _handle;

		public:
			AprBaton(T value)
			{
				if (value == nullptr)
					throw gcnew ArgumentNullException("value");

				_handle = GCHandle::Alloc(value, System::Runtime::InteropServices::GCHandleType::WeakTrackResurrection);
			}

		private:
			!AprBaton()
			{
				if (_handle.IsAllocated)
					_handle.Free();
			}

			~AprBaton()
			{
				if (_handle.IsAllocated)
					_handle.Free();
			}
		public:
			static T Get(IntPtr value)
			{
				return (T)GCHandle::FromIntPtr(value).Target;
			}

			static T Get(void* ptr)
			{
				return (T)GCHandle::FromIntPtr((IntPtr)ptr).Target;
			}


			property void* Handle
			{
				[System::Diagnostics::DebuggerStepThrough]
				void *get()
				{
					return (void *)GCHandle::ToIntPtr(_handle);
				}
			}
		};
	}
}