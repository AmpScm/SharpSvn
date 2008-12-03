// $Id$
// Copyright (c) SharpSvn Project 2007
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

using namespace System;

namespace SharpSvn {

using System::ComponentModel::DescriptionAttribute;

#define SVN_ERRDEF(x,y,z) [DescriptionAttribute(z)] x = x,
#define SVN_ERROR_BUILD_ARRAY
#pragma warning(disable: 4634)

#define DOXYGEN_SHOULD_SKIP_THIS
#define SVN_ERROR_START public enum class SvnErrorCode { None=0,
#define SVN_ERROR_END };

#include "svn_error_codes.h"

#undef SVN_ERRDEF
#undef SVN_ERROR_BUILD_ARRAY
#undef DOXYGEN_SHOULD_SKIP_THIS
#undef SVN_ERROR_START
#undef SVN_ERROR_END

/// <summary>Gets the error category of the SvnException</summary>
public enum class SvnErrorCategory
{
	OperatingSystem=-1,
	None=0,
	Bad,
	Xml,
	Io,
	Stream,
	Node,
	Entry,
	Wc,
	Fs,
	Repository,
	RepositoryAccess,
	RepositoryAccessDav,
	RepositoryAccessLocal,
	SvnDiff,
	ApacheModule,
	Client,
	Misc,
	CommandLine,
	RepositoryAccessSvn,
	Authentication,
	Authorization,
	Diff,
	RepositoryAccessSerf,
};

	[Serializable]
	public ref class SvnException : public System::Exception
	{
	private:
		initonly int _errorCode;

		static String^ GetErrorText(svn_error_t *error);

		static Exception^ GetInnerException(svn_error_t *error)
		{
			if (error && error->child)
				return Create(error->child, false);
			else
				return nullptr;
		}

	internal:
		static SvnException^ Create(svn_error_t *error);
		static Exception^ Create(svn_error_t *error, bool clearError);
		static svn_error_t* CreateExceptionSvnError(String^ origin, Exception^ exception);

	private protected:
		SvnException(svn_error_t *error)
			: Exception(GetErrorText(error), GetInnerException(error))
		{
			if (error)
				_errorCode = error->apr_err;
		}

	protected:
		SvnException(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context)
			: Exception(info, context)
		{
			if (!info)
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

		/// <summary>Gets the raw subversion error code</summary>
		property int SubversionErrorCode
		{
			int get()
			{
				return _errorCode;
			}
		}

		/// <summary>Gets the operating system error code when there is one 
		/// (Only valid if SvnErrorCategory returns <See cref="SharpSvn::SvnErrorCategory::OperatingSystem" />)
		/// </summary>
		property int OperatingSystemErrorCode
		{
			int get()
			{
				if (_errorCode >= APR_OS_START_STATUS && _errorCode < (APR_OS_START_STATUS + APR_OS_ERRSPACE_SIZE))
					return APR_TO_OS_ERROR(_errorCode);
				else
					return -1;
			}
		}

		/// <summary>Gets the raw subversion error code casted to a <see cref="SharpSvn::SvnErrorCode" /></summary>
		property SharpSvn::SvnErrorCode SvnErrorCode
		{
			SharpSvn::SvnErrorCode get()
			{
				return (SharpSvn::SvnErrorCode)SubversionErrorCode;
			}
		}

		property SharpSvn::SvnErrorCategory SvnErrorCategory
		{
			SharpSvn::SvnErrorCategory get()
			{
				if (_errorCode >= SVN_ERR_BAD_CATEGORY_START && _errorCode < SVN_ERR_RA_SERF_CATEGORY_START + SVN_ERR_CATEGORY_SIZE)
					return (SharpSvn::SvnErrorCategory)(_errorCode / SVN_ERR_CATEGORY_SIZE);
				else if (_errorCode >= APR_OS_START_STATUS && _errorCode < (APR_OS_START_STATUS + APR_OS_ERRSPACE_SIZE))
					return SharpSvn::SvnErrorCategory::OperatingSystem;
				else
					return SharpSvn::SvnErrorCategory::None;

				svn_err_best_message(
			}
		}

		/// <summary>Gets the root cause of the exception; commonly the most <see cref="InnerException" /></summary>
		property Exception^ RootCause
		{
			Exception^ get()
			{
				Exception^ e = this;
				while (e->InnerException)
					e = e->InnerException;

				return e;
			}
		}

		Exception^ GetCause(System::Type^ exceptionType)
		{
			if (!exceptionType)
				throw gcnew ArgumentNullException("exceptionType");
			else if (!Exception::typeid->IsAssignableFrom(exceptionType))
				throw gcnew ArgumentOutOfRangeException("exceptionType");

			Exception^ e = this;

			while (e)
			{
				if (exceptionType->IsAssignableFrom(e->GetType()))
					return e;

				e = e->InnerException;
			}

			return nullptr;
		}

		generic<typename T> where T : Exception
		T GetCause()
		{
			return (T)GetCause(T::typeid);
		}

	public:
		[System::Security::Permissions::SecurityPermission(System::Security::Permissions::SecurityAction::LinkDemand, Flags = System::Security::Permissions::SecurityPermissionFlag::SerializationFormatter)]
		virtual void GetObjectData(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context) override
		{
			if (!info)
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
	DECLARE_SVN_EXCEPTION_TYPE(SvnBreakIterationException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnUnknownChangeListException, SvnException);

	DECLARE_SVN_EXCEPTION_TYPE(SvnFormatException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnXmlException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnIOException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnStreamException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnNodeException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnEntryException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnEntryNotFoundException, SvnEntryException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnWorkingCopyException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnWorkingCopyLockException, SvnWorkingCopyException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnFileSystemException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnFileSystemNodeTypeException, SvnFileSystemException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnFileSystemLockException, SvnFileSystemException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnFileSystemOutOfDateException, SvnFileSystemException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnRepositoryException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnRepositoryIOException, SvnException); // Does not extent Repository Exception (= RA library exception)
	DECLARE_SVN_EXCEPTION_TYPE(SvnRepositoryHookException, SvnRepositoryException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnAuthenticationException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnAuthorizationException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnDiffException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnClientApiException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnDiskFullException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnIllegalTargetException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnObstructedUpdateException, SvnWorkingCopyException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnInvalidNodeKindException, SvnWorkingCopyException);

	DECLARE_SVN_EXCEPTION_TYPE(SvnUnsupportedFeatureException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnUnknownCapabilityException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnChecksumMismatchException, SvnException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnReservedNameUsedException, SvnException);

	/// <summary>SharpSvn SvnClient exception</summary>
	DECLARE_SVN_EXCEPTION_TYPE(SvnClientException, SvnException);

	DECLARE_SVN_EXCEPTION_TYPE(SvnClientNoVersionedPathException, SvnClientException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnClientNoVersionedParentException, SvnClientException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnClientUnrelatedResourcesException, SvnClientException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnClientMissingLockTokenException, SvnClientException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnClientNotReadyToMergeException, SvnClientException);

	DECLARE_SVN_EXCEPTION_TYPE(SvnClientNodeKindException, SvnClientException);
	DECLARE_SVN_EXCEPTION_TYPE(SvnClientBinaryFileException, SvnClientException);

	/// <summary>SharpSvn configuration loader exception</summary>
	DECLARE_SVN_EXCEPTION_TYPE(SvnClientConfigurationException, SvnClientException);
}
