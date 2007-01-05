
#pragma once

#include "AprPool.h"

using namespace System;
using namespace TurtleSvn::Apr;

namespace TurtleSvn {
	public ref class SvnClientContext : public SvnHandleBase
	{
		svn_client_ctx_t *_ctx;
		AprPool^ _pool;
	public:
		SvnClientContext(AprPool^ pool);

	private:
		~SvnClientContext();

	internal:
		property svn_client_ctx_t *CtxHandle
		{
			svn_client_ctx_t *get();
		}
	};
}
