
#pragma once

using namespace System;

#include "SvnBase.h"


namespace TurtleSvn {
	namespace Apr {

		[Serializable]
		ref class AprException : public System::Runtime::InteropServices::ExternalException
		{
		private:
			apr_status_t _status;

		static String^ GetErrorText(apr_status_t status)
		{
			char buffer[256];

			String^ result = TurtleSvn::Apr::SvnBase::Utf8_PtrToString(apr_strerror(status, buffer, sizeof(buffer)));

			return result;
		}

		public:
			AprException(apr_status_t status)
				: ExternalException(GetErrorText(status))
			{
				_status = status;
			}
			
		};
	}
}