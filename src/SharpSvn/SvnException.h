
#pragma once

#include "AprException.h"

using namespace System;

namespace SharpSvn {

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
				return SharpSvn::Apr::SvnBase::Utf8_PtrToString(error->message);
			}
			catch(Exception^)
			{
				return "Subversion error: Unable to retrieve error text";
			}
		}

	internal:
		static SvnException^ Create(svn_error_t *error);
		

	internal:
		SvnException(svn_error_t *error)
			: ExternalException(GetErrorText(error))
		{
			_error = error;
		}

	};

	public ref class SvnAuthorizationException : public SvnException
	{
	internal:
		SvnAuthorizationException(svn_error_t *error)
			: SvnException(error)
		{
		}

	};

	public ref class SvnOperationCanceledException : public SvnException
	{
	internal:
		SvnOperationCanceledException(svn_error_t *error)
			: SvnException(error)
		{
		}

	};
}
