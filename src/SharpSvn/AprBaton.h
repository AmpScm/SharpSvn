// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

namespace SharpSvn {
	namespace Apr
	{
		using System::Runtime::InteropServices::GCHandle;

		generic<typename T>
		ref class AprBaton : public SharpSvn::Apr::SvnBase, public IDisposable
		{
			GCHandle _handle;

		public:
			AprBaton(T value)
			{
				if(value == nullptr)
					throw gcnew ArgumentNullException("value");

				_handle = GCHandle::Alloc(value, System::Runtime::InteropServices::GCHandleType::WeakTrackResurrection);
			}

		private:
			!AprBaton()
			{
				if(_handle.IsAllocated)
					_handle.Free();
			}

			~AprBaton()
			{
				if(_handle.IsAllocated)
					_handle.Free();
			}
		public:
			static T Get(IntPtr value)
			{
				return (T)GCHandle::FromIntPtr(value).Target;
			}


			property IntPtr Handle
			{
				[System::Diagnostics::DebuggerStepThrough]
				IntPtr get()
				{
					return GCHandle::ToIntPtr(_handle);
				}
			}
		};
	}
}