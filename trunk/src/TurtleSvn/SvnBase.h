
#pragma once

using namespace System;


namespace TurtleSvn {
	namespace Apr {
		ref class AprPool;

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
			static String^ Utf8_PtrToString(const char *ptr);
			static String^ Utf8_PtrToString(const char *ptr, int length);

			static const char* Utf8_StringToPtr(String ^value, TurtleSvn::Apr::AprPool^ pool);
		};

		public ref class SvnHandleBase abstract : public System::Runtime::ConstrainedExecution::CriticalFinalizerObject
		{
		private:
			// Initializes runtime
			static SvnHandleBase();

		protected:
			SvnHandleBase();

		private protected:
			static String^ Utf8_PtrToString(const char *ptr);
			static String^ Utf8_PtrToString(const char *ptr, int length);

			static const char* Utf8_StringToPtr(String ^value, TurtleSvn::Apr::AprPool^ pool);
		};
	}
}
