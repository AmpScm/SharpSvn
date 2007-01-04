
#pragma once

namespace QQn {
	namespace Svn {
		using System::Runtime::InteropServices::GCHandle;

		generic<typename T>
		ref class SvnBatton : public IDisposable
		{
			GCHandle _handle;

		public:
			SvnBatton(T value)
			{
				if(value == nullptr)
					throw gcnew ArgumentNullException("value");

				_handle = GCHandle::Alloc(value, System::Runtime::InteropServices::GCHandleType::WeakTrackResurrection);
			}

		private:
			!SvnBatton()
			{
				if(_handle.IsAllocated)
					_handle.Free();
			}

			~SvnBatton()
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
				IntPtr get()
				{
					return GCHandle::ToIntPtr(_handle);
				}
			}
		};
	}
}