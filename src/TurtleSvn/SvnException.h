
#pragma once

#include "AprException.h"

using namespace System;

namespace QQn {
	namespace Svn {

		[Serializable]
		public ref class SvnException : public System::Runtime::InteropServices::ExternalException
		{
		private:
			svn_error_t *_error;

		static String^ GetErrorText(svn_error_t *error)
		{
			if(!error)
				return "";

			return QQn::Svn::SvnBase::PtrToStringUtf8(error->message);
		}

		public:
			SvnException(svn_error_t *error)
				: ExternalException(GetErrorText(error))
			{
				_error = error;
			}
			
		};
	}
}