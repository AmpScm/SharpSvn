
#pragma once

#include "AprException.h"

using namespace System;

namespace SharpSvn {

	[Serializable]
	public ref class SvnException : public System::Exception
	{
	private:
		initonly int _errorCode;

		static String^ GetErrorText(svn_error_t *error);

		static Exception^ GetInnerException(svn_error_t *error)
		{
			if(error && error->child)
				return Create(error->child, false);
			else
				return nullptr;
		}

	internal:
		static SvnException^ Create(svn_error_t *error);
		static svn_error_t* CreateExceptionSvnError(String^ origin, Exception^ exception);

	private:
		static Exception^ Create(svn_error_t *error, bool clearError);

	private protected:
		SvnException(svn_error_t *error)
			: Exception(GetErrorText(error), GetInnerException(error))
		{
			if(error)
				_errorCode = error->apr_err;
		}

	protected:
		SvnException(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context)
			: Exception(info, context)
		{
			if(!info)
				throw gcnew ArgumentNullException("info");
			UNUSED_ALWAYS(context);

			_errorCode = info->GetInt32("SvnErrorValue");
		}

	public:
		SvnException()
		{
		}

		SvnException(String^ message)
			: Exception(message)
		{
		}

		SvnException(String^ message, Exception^ inner)
			: Exception(message, inner)
		{
		}

		property int ErrorCode
		{
			int get()
			{
				return _errorCode;
			}
		}

	public:
		[System::Security::Permissions::SecurityPermission(System::Security::Permissions::SecurityAction::LinkDemand, Flags = System::Security::Permissions::SecurityPermissionFlag::SerializationFormatter)]
		virtual void GetObjectData(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context) override
		{
			if(!info)
				throw gcnew ArgumentNullException("info");
			Exception::GetObjectData(info, context);

			info->AddValue("SvnErrorValue", _errorCode);
		}
	};

	//////////// Generic Subversion exception wrappers

#define DECLARE_SVN_EXCEPTION_TYPE(type, parentType)					\
	[Serializable]														\
	public ref class type : public parentType							\
	{																	\
	internal:															\
		type(svn_error_t *error)										\
			: parentType(error)											\
		{}																\
	protected:															\
		type(System::Runtime::Serialization::SerializationInfo^ info,	\
		System::Runtime::Serialization::StreamingContext context)		\
			: parentType(info, context)									\
		{}																\
	public:																\
		type()															\
		{}																\
		type(String^ message)											\
			: parentType(message)										\
		{}																\
		type(String^ message, Exception^ inner)							\
			: parentType(message, inner)								\
		{}																\
	};

	DECLARE_SVN_EXCEPTION_TYPE(SvnOperationCanceledException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnOperationCompletedException, SvnException);


	DECLARE_SVN_EXCEPTION_TYPE(SvnFormatException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnXmlException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnIOException, SvnException);
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
