#pragma once

namespace SharpSvn {
	namespace Implementation {

		public ref class SvnFsOperationRetryOverride : public SvnBase
		{
			initonly int _maxRetries;
			initonly bool _intOnly;
			[ThreadStatic]
			static SvnFsOperationRetryOverride^ _handler;
			SvnFsOperationRetryOverride^ _oldHandler;

		private:
			static SvnFsOperationRetryOverride();
			~SvnFsOperationRetryOverride();

		public:
			SvnFsOperationRetryOverride(int maxRetries);

		private:
			bool OnRetryLoopInvocation(int nr, int error, int osError, const char *expr);

		protected:
			virtual bool OnRetryLoopInvocation(int nr, SharpSvn::SvnErrorCode error, int osError, String ^expr);

		internal:
			static int RetryLoopHandler(int n, int err, int os_err, const char *expr);
		};
	}
}