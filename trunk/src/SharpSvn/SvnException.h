// $Id$
//
// Copyright 2007-2008 The SharpSvn Project
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#pragma once

using namespace System;

#include "SvnErrorCodes.h"

namespace SharpSvn {

	[Serializable]
	public ref class SvnException : public System::Exception
	{
		initonly int _errorCode;
		[NonSerialized]
		const char* _pFile;
		String^ _file;
		initonly int _line;
		[NonSerialized]
		Object ^_targets;

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
			{
				_errorCode = error->apr_err;
				_line = error->line;
				_pFile = error->file;
			}
		}

		SvnException(String^ message, String^ file, int line)
			: Exception(message)
		{
			_file = file;
			_line = line;
		}

	protected:
		SvnException(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context)
			: Exception(info, context)
		{
			if (!info)
				throw gcnew ArgumentNullException("info");
			UNUSED_ALWAYS(context);

			_errorCode = info->GetInt32("SvnErrorValue");
			_file = info->GetString("_file");
			_line = info->GetInt32("_line");
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

		/// <summary>Gets the operating system error code when there is one
		/// (Only valid if SvnErrorCategory returns <See cref="SharpSvn::SvnErrorCategory::OperatingSystem" />)
		/// </summary>
		property SvnWindowsErrorCode WindowsErrorCode
		{
			SvnWindowsErrorCode get()
			{
				if (_errorCode >= APR_OS_START_STATUS && _errorCode < (APR_OS_START_STATUS + APR_OS_ERRSPACE_SIZE))
					return (SvnWindowsErrorCode)APR_TO_OS_ERROR(_errorCode);
				else
					return (SvnWindowsErrorCode)0;
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
		property String^ File
		{
			String^ get()
			{
				if (!_file && _pFile)
				{
					const char* pf = _pFile;
					_pFile = nullptr;
					try
					{
						_file = gcnew String(pf);
					}
					catch(AccessViolationException^)
					{
						/* Subversion will always set file via __FILE__ which comes from
						   a readonly memory segment so this should never crash, but just in case... */
					}
				}
				return _file;
			}
		}

		property int Line
		{
			int get()
			{
				return _line;
			}
		}

		/// <summary>When not NULL, contains a String, Uri or SvnTarget, or an IEnumberable of one of these,
		/// containing (some of) the targets of the command executed. This to help debugging issues from just
		/// handling the exceptions</summary>
		property Object^ Targets
		{
			Object^ get()
			{
				return _targets;
			}
		internal:
			void set(Object ^value)
			{
				_targets = value;
			}
		}

	public:
		[System::Security::Permissions::SecurityPermission(System::Security::Permissions::SecurityAction::LinkDemand, Flags = System::Security::Permissions::SecurityPermissionFlag::SerializationFormatter)]
		virtual void GetObjectData(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context) override
		{
			if (!info)
				throw gcnew ArgumentNullException("info");
			Exception::GetObjectData(info, context);

			info->AddValue("SvnErrorValue", _errorCode);
			info->AddValue("_file", File);
			info->AddValue("_line", Line);
		}
	};

	[Serializable]
	public ref class SvnMalfunctionException sealed : SvnException
	{
	public:
		SvnMalfunctionException()
		{
		}

		SvnMalfunctionException(String^ message)
			: SvnException(message)
		{
		}

		SvnMalfunctionException(String^ message, Exception^ inner)
			: SvnException(message, inner)
		{
		}

		SvnMalfunctionException(String^ message, String^ file, int line)
			: SvnException(String::Format(SharpSvnStrings::SvnMalfunctionPrefix, message, file, line), file, line)
		{
		}

	protected:
		SvnMalfunctionException(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context)
			: SvnException(info, context)
		{
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
