
#pragma once

#include "AprPool.h"

using namespace System;
using namespace QQn::Apr;

namespace QQn {
	namespace Svn {
		public ref class SvnClientContext : public SvnHandleBase
		{
			svn_client_ctx_t *_handle;
		public:
			SvnClientContext(AprPool^ pool);

		private:
			~SvnClientContext();
		
		internal:
			property svn_client_ctx_t *Handle
			{
				svn_client_ctx_t *get()
				{
					if(!_handle)
						throw gcnew ObjectDisposedException("SvnClientContext");

					return _handle;
				}
			}
		};
	}
}