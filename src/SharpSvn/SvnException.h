
#pragma once

#include "AprException.h"

using namespace System;

namespace SharpSvn {

	[Serializable]
	public ref class SvnException : public System::Exception
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
				return Create(error->child, false);
			else
				return nullptr;
		}

	internal:
		static SvnException^ Create(svn_error_t *error);

	private:
		static Exception^ Create(svn_error_t *error, bool clearError);

	private protected:
		SvnException(svn_error_t *error)
			: Exception(GetErrorText(error), GetInnerException(error))
		{
			_error = error;
		}

	protected:
		SvnException(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context)
			: Exception(info, context)
		{
			UNUSED_ALWAYS(info);
			UNUSED_ALWAYS(context);
		}
	};

	//////////// Generic Subversion exception wrappers

#define DECLARE_SVN_EXCEPTION_TYPE(type, parentType)			\
	[Serializable]												\
	public ref class type : public parentType					\
	{															\
	internal:													\
		type(svn_error_t *error)								\
			: parentType(error)									\
		{}														\
	protected:													\
	type(System::Runtime::Serialization::SerializationInfo^ info,\
	System::Runtime::Serialization::StreamingContext context)	\
		: parentType(info, context)								\
	{}															\
	};

	DECLARE_SVN_EXCEPTION_TYPE(SvnOperationCanceledException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnOperationCompletedException, SvnException);


	DECLARE_SVN_EXCEPTION_TYPE(SvnFormatException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnXmlException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnIoException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnStreamException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnNodeException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnEntryException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnWorkingCopyException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnWorkingCopyLockException, SvnWorkingCopyException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnFileSystemException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnFileSystemLockException, SvnFileSystemException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnRepositoryException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnRepositoryIoException, SvnException); // Does not extent Repository Exception (= RA library exception)
	DECLARE_SVN_EXCEPTION_TYPE(SvnAuthenticationException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnAuthorizationException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnDiffException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnClientApiException, SvnException);
}
