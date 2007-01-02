
#pragma once

#include "SvnClientContext.h"

namespace QQn {
	namespace Svn {
		public ref class SvnClient : public SvnClientContext
		{
			AprPool^ _pool;
		public:
			SvnClient();
			SvnClient(AprPool^ pool);

		private:
			~SvnClient();
		public:
			!SvnClient();
		};
	}
}