
#pragma once

using namespace System;


namespace QQn {
	namespace Svn {

		public ref class SvnBase abstract
		{
		private:
			// Initializes runtime
			static SvnBase();

		internal:
			static void EnsureLoaded();

		protected:
			SvnBase();

		internal:
			static String^ PtrToStringUtf8(const char *ptr);
			static String^ PtrToStringUtf8(const char *ptr, int length);
		};

		public ref class SvnHandleBase abstract : public System::Runtime::ConstrainedExecution::CriticalFinalizerObject
		{
		private:
			// Initializes runtime
			static SvnHandleBase();

		protected:
			SvnHandleBase();

		internal:
			static String^ PtrToStringUtf8(const char *ptr);
			static String^ PtrToStringUtf8(const char *ptr, int length);
		};
	}
}