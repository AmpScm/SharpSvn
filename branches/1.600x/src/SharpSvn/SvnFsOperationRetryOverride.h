#pragma once

namespace SharpSvn {
	namespace Implementation {

		using System::Collections::Generic::Stack;

		public ref class SvnFsOperationRetryOverride : public SvnBase
		{
			initonly int _maxRetries;
			initonly bool _intOnly;
			[ThreadStatic]
			static Stack<SvnFsOperationRetryOverride^>^ _handlers;
			Stack<SvnFsOperationRetryOverride^>^ _onStack;

		private:
			static SvnFsOperationRetryOverride();
			~SvnFsOperationRetryOverride();

		public:
			SvnFsOperationRetryOverride(int maxRetries);

		private:
			bool OnRetryLoopInvocation(int nr, int error, int osError, const char *expr);

		protected:
			virtual bool OnRetryLoopInvocation(int nr, SharpSvn::SvnAprErrorCode aprError, SharpSvn::SvnWindowsErrorCode windowsError, String ^expr);

		internal:
			static int RetryLoopHandler(int n, int err, int os_err, const char *expr);
		};
	}
}