
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

		static Exception^ GetInnerException(svn_error_t *error)
		{
			if(error && error->child)
				return Create(error->child);
			else
				return nullptr;
		}

	internal:
		static SvnException^ Create(svn_error_t *error);
		

	private protected:
		SvnException(svn_error_t *error)
			: ExternalException(GetErrorText(error), GetInnerException(error))
		{
			_error = error;
		}

	};

	public ref class SvnAuthorizationException : public SvnException
	{
	private protected:
		SvnAuthorizationException(svn_error_t *error)
			: SvnException(error)
		{
		}

	internal:
		static SvnAuthorizationException^ Create(svn_error_t *error)
		{
			return gcnew SvnAuthorizationException(error);
		}
	};

	public ref class SvnOperationCanceledException : public SvnException
	{
	private protected:
		SvnOperationCanceledException(svn_error_t *error)
			: SvnException(error)
		{
		}

	internal:
		static SvnOperationCanceledException^ Create(svn_error_t *error)
		{
			return gcnew SvnOperationCanceledException(error);
		}

	};
}
