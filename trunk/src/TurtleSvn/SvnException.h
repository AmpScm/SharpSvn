
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

			try
			{
				return QQn::Svn::SvnBase::Utf8_PtrToString(error->message);
			}
			catch(Exception^)
			{
				return "Subversion error: Unable to retrieve error text";
			}
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