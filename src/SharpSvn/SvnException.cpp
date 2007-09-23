#include "stdafx.h"

#include "SvnAll.h"
#include <svn_config.h>

using namespace SharpSvn;
using namespace SharpSvn::Apr;

#define MANAGED_EXCEPTION_PREFIX "Forwarded Managed Inner Exception/SharpSvn/Handle="

class SvnExceptionContainer sealed
{
private:
	static const INT_PTR _idValue = 0xFF00FFEE;
	INT_PTR _id;
	gcroot<Exception^> _exception;

private:
	static apr_status_t cleanup_handler(void* data)
	{
		SvnExceptionContainer* ptr = reinterpret_cast<SvnExceptionContainer*>(data);

		System::Diagnostics::Debug::Assert(ptr->_id == _idValue);

		if(ptr->_id == _idValue)
		{
			delete ptr;
		}

		return 0;
	}

public:
	SvnExceptionContainer(Exception^ ex, apr_pool_t* pool)
	{
		_id = _idValue;
		_exception = ex;
		
		apr_pool_cleanup_register(pool, this, cleanup_handler, cleanup_handler);
		
		_exception = ex;
	}

	static Exception^ Fetch(const SvnExceptionContainer* container)
	{
		System::Diagnostics::Debug::Assert(container && container->_id == _idValue);

		if(container->_id == _idValue)
			return container->_exception;
		else
			return nullptr;
	}

private:
	~SvnExceptionContainer()
	{		
		System::Diagnostics::Debug::Assert(_id == _idValue);
		_id = 0;
		_exception = nullptr;
	}
};


svn_error_t* SvnException::CreateExceptionSvnError(String^ origin, Exception^ exception)
{
	AprPool tmpPool;

	svn_error_t *innerError = nullptr;

	if(exception)
	{
		svn_error_t *creator = svn_error_create(SVN_ERR_CANCELLED, nullptr, "{Managed Exception Blob}");

		if(creator->pool)
		{
			char ptrBuffer[32];

			SvnExceptionContainer *ex = new SvnExceptionContainer(exception, creator->pool);
			
			sprintf_s(ptrBuffer, sizeof(ptrBuffer), "%p", (void*)ex);			

			char* forwardData = apr_pstrcat(creator->pool, MANAGED_EXCEPTION_PREFIX, ptrBuffer, NULL);

			innerError = svn_error_create(SVN_ERR_CANCELLED, creator, forwardData);
		}
	}

	return svn_error_create(SVN_ERR_CANCELLED, innerError, tmpPool.AllocString(String::Format(System::Globalization::CultureInfo::InvariantCulture, "Operation canceled. Exception occured in {0}", origin)));
}



[module: SuppressMessage("Microsoft.Design", "CA1031:DoNotCatchGeneralExceptionTypes", Scope="member", Target="SharpSvn.SvnException.GetErrorText(svn_error_t*):System.String")];
String^ SvnException::GetErrorText(svn_error_t *error)
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


SvnException^ SvnException::Create(svn_error_t *error)
{
	return static_cast<SvnException^>(Create(error, true));
}

[module: SuppressMessage("Microsoft.Design", "CA1031:DoNotCatchGeneralExceptionTypes", Scope="member", Target="SharpSvn.SvnException.Create(svn_error_t*,System.Boolean):System.Exception")];
Exception^ SvnException::Create(svn_error_t *error, bool clearError)
{
	if(!error)
		return nullptr;

	static const int prefixLength = (int)strlen(MANAGED_EXCEPTION_PREFIX);

	try
	{
		switch(error->apr_err)
		{
		case SVN_ERR_BAD_FILENAME:
		case SVN_ERR_BAD_URL:
		case SVN_ERR_BAD_DATE:
		case SVN_ERR_BAD_MIME_TYPE:
		case SVN_ERR_BAD_PROPERTY_VALUE:
		case SVN_ERR_BAD_VERSION_FILE_FORMAT:
			return gcnew SvnFormatException(error);
		case SVN_ERR_XML_ATTRIB_NOT_FOUND:
		case SVN_ERR_XML_MISSING_ANCESTRY:
		case SVN_ERR_XML_UNKNOWN_ENCODING:
		case SVN_ERR_XML_MALFORMED:
		case SVN_ERR_XML_UNESCAPABLE_DATA:
			return gcnew SvnXmlException(error);
		case SVN_ERR_IO_INCONSISTENT_EOL:
		case SVN_ERR_IO_UNKNOWN_EOL:
		case SVN_ERR_IO_UNIQUE_NAMES_EXHAUSTED:
		case SVN_ERR_IO_WRITE_ERROR:
			return gcnew SvnIOException(error);
		case SVN_ERR_STREAM_UNEXPECTED_EOF:
		case SVN_ERR_STREAM_MALFORMED_DATA:
		case SVN_ERR_STREAM_UNRECOGNIZED_DATA:
			return gcnew SvnStreamException(error);
		case SVN_ERR_NODE_UNKNOWN_KIND:
		case SVN_ERR_NODE_UNEXPECTED_KIND:
			return gcnew SvnNodeException(error);
		case SVN_ERR_ENTRY_NOT_FOUND:
		case SVN_ERR_ENTRY_EXISTS:
		case SVN_ERR_ENTRY_MISSING_REVISION:
		case SVN_ERR_ENTRY_MISSING_URL:
		case SVN_ERR_ENTRY_ATTRIBUTE_INVALID:
			return gcnew SvnEntryException(error);
		case SVN_ERR_WC_OBSTRUCTED_UPDATE:
		case SVN_ERR_WC_NOT_DIRECTORY:
		case SVN_ERR_WC_NOT_FILE:
		case SVN_ERR_WC_BAD_ADM_LOG:
		case SVN_ERR_WC_PATH_NOT_FOUND:
		case SVN_ERR_WC_NOT_UP_TO_DATE:
		case SVN_ERR_WC_LEFT_LOCAL_MOD:
		case SVN_ERR_WC_SCHEDULE_CONFLICT:
		case SVN_ERR_WC_PATH_FOUND:
		case SVN_ERR_WC_FOUND_CONFLICT:
		case SVN_ERR_WC_CORRUPT:
		case SVN_ERR_WC_CORRUPT_TEXT_BASE:
		case SVN_ERR_WC_NODE_KIND_CHANGE:
		case SVN_ERR_WC_INVALID_OP_ON_CWD:
		case SVN_ERR_WC_BAD_ADM_LOG_START:
		case SVN_ERR_WC_UNSUPPORTED_FORMAT:
		case SVN_ERR_WC_BAD_PATH:
		case SVN_ERR_WC_INVALID_SCHEDULE:
		case SVN_ERR_WC_INVALID_RELOCATION:
		case SVN_ERR_WC_INVALID_SWITCH:
		case SVN_ERR_WC_MISMATCHED_CHANGELIST:
		case SVN_ERR_WC_CONFLICT_RESOLVER_FAILURE:
			return gcnew SvnWorkingCopyException(error);
		case SVN_ERR_WC_LOCKED:
		case SVN_ERR_WC_NOT_LOCKED:
			return gcnew SvnWorkingCopyLockException(error);
		case SVN_ERR_FS_GENERAL:
		case SVN_ERR_FS_CLEANUP:
		case SVN_ERR_FS_ALREADY_OPEN:
		case SVN_ERR_FS_NOT_OPEN:
		case SVN_ERR_FS_CORRUPT:
		case SVN_ERR_FS_PATH_SYNTAX:
		case SVN_ERR_FS_NO_SUCH_REVISION:
		case SVN_ERR_FS_NO_SUCH_TRANSACTION:
		case SVN_ERR_FS_NO_SUCH_ENTRY:
		case SVN_ERR_FS_NO_SUCH_REPRESENTATION:
		case SVN_ERR_FS_NO_SUCH_STRING:
		case SVN_ERR_FS_NO_SUCH_COPY:
		case SVN_ERR_FS_TRANSACTION_NOT_MUTABLE:
		case SVN_ERR_FS_NOT_FOUND:
		case SVN_ERR_FS_ID_NOT_FOUND:
		case SVN_ERR_FS_NOT_ID:
		case SVN_ERR_FS_NOT_DIRECTORY:
		case SVN_ERR_FS_NOT_FILE:
		case SVN_ERR_FS_NOT_SINGLE_PATH_COMPONENT:
		case SVN_ERR_FS_NOT_MUTABLE:
		case SVN_ERR_FS_ALREADY_EXISTS:
		case SVN_ERR_FS_ROOT_DIR:
		case SVN_ERR_FS_NOT_TXN_ROOT:
		case SVN_ERR_FS_NOT_REVISION_ROOT:
		case SVN_ERR_FS_CONFLICT:
		case SVN_ERR_FS_REP_CHANGED:
		case SVN_ERR_FS_REP_NOT_MUTABLE:
		case SVN_ERR_FS_MALFORMED_SKEL:
		case SVN_ERR_FS_TXN_OUT_OF_DATE:
		case SVN_ERR_FS_BERKELEY_DB:
		case SVN_ERR_FS_BERKELEY_DB_DEADLOCK:
		case SVN_ERR_FS_TRANSACTION_DEAD:
		case SVN_ERR_FS_TRANSACTION_NOT_DEAD:
		case SVN_ERR_FS_UNKNOWN_FS_TYPE:
		case SVN_ERR_FS_NO_USER:
		case SVN_ERR_FS_OUT_OF_DATE:		
		case SVN_ERR_FS_UNSUPPORTED_FORMAT:
		case SVN_ERR_FS_REP_BEING_WRITTEN:
		case SVN_ERR_FS_TXN_NAME_TOO_LONG:
		case SVN_ERR_FS_SQLITE_ERROR:
			return gcnew SvnFileSystemException(error);
		case SVN_ERR_FS_PATH_ALREADY_LOCKED:
		case SVN_ERR_FS_PATH_NOT_LOCKED:
		case SVN_ERR_FS_BAD_LOCK_TOKEN:
		case SVN_ERR_FS_NO_LOCK_TOKEN:
		case SVN_ERR_FS_LOCK_OWNER_MISMATCH:
		case SVN_ERR_FS_NO_SUCH_LOCK:
		case SVN_ERR_FS_LOCK_EXPIRED:
			return gcnew SvnFileSystemLockException(error);
		case SVN_ERR_REPOS_LOCKED:
		case SVN_ERR_REPOS_HOOK_FAILURE:
		case SVN_ERR_REPOS_BAD_ARGS:
		case SVN_ERR_REPOS_NO_DATA_FOR_REPORT:
		case SVN_ERR_REPOS_BAD_REVISION_REPORT:
		case SVN_ERR_REPOS_UNSUPPORTED_VERSION:
		case SVN_ERR_REPOS_DISABLED_FEATURE:
		case SVN_ERR_REPOS_POST_COMMIT_HOOK_FAILED:
		case SVN_ERR_REPOS_POST_LOCK_HOOK_FAILED:
		case SVN_ERR_REPOS_POST_UNLOCK_HOOK_FAILED:
			return gcnew SvnRepositoryException(error);
		case SVN_ERR_RA_ILLEGAL_URL:
		case SVN_ERR_RA_NOT_AUTHORIZED:
		case SVN_ERR_RA_UNKNOWN_AUTH:
		case SVN_ERR_RA_NOT_IMPLEMENTED:
		case SVN_ERR_RA_OUT_OF_DATE:
		case SVN_ERR_RA_NO_REPOS_UUID:
		case SVN_ERR_RA_UNSUPPORTED_ABI_VERSION:
		case SVN_ERR_RA_NOT_LOCKED:
		case SVN_ERR_RA_DAV_SOCK_INIT:
		case SVN_ERR_RA_DAV_CREATING_REQUEST:
		case SVN_ERR_RA_DAV_REQUEST_FAILED:
		case SVN_ERR_RA_DAV_OPTIONS_REQ_FAILED:
		case SVN_ERR_RA_DAV_PROPS_NOT_FOUND:
		case SVN_ERR_RA_DAV_ALREADY_EXISTS:
		case SVN_ERR_RA_DAV_INVALID_CONFIG_VALUE:
		case SVN_ERR_RA_DAV_PATH_NOT_FOUND:
		case SVN_ERR_RA_DAV_PROPPATCH_FAILED:
		case SVN_ERR_RA_DAV_MALFORMED_DATA:
		case SVN_ERR_RA_DAV_RESPONSE_HEADER_BADNESS:
		case SVN_ERR_RA_DAV_RELOCATED:
		case SVN_ERR_RA_LOCAL_REPOS_NOT_FOUND:
		case SVN_ERR_RA_LOCAL_REPOS_OPEN_FAILED:
		case SVN_ERR_RA_SVN_CMD_ERR:
		case SVN_ERR_RA_SVN_UNKNOWN_CMD:
		case SVN_ERR_RA_SVN_CONNECTION_CLOSED:
		case SVN_ERR_RA_SVN_IO_ERROR:
		case SVN_ERR_RA_SVN_MALFORMED_DATA:
		case SVN_ERR_RA_SVN_REPOS_NOT_FOUND:
		case SVN_ERR_RA_SVN_BAD_VERSION:
		case SVN_ERR_RA_SVN_NO_MECHANISMS:
			return gcnew SvnRepositoryIOException(error);

		case SVN_ERR_AUTHN_CREDS_UNAVAILABLE:
		case SVN_ERR_AUTHN_NO_PROVIDER:
		case SVN_ERR_AUTHN_PROVIDERS_EXHAUSTED:
		case SVN_ERR_AUTHN_CREDS_NOT_SAVED:
		case SVN_ERR_AUTHN_FAILED:
			return gcnew SvnAuthenticationException(error);
		case SVN_ERR_AUTHZ_ROOT_UNREADABLE:
		case SVN_ERR_AUTHZ_UNREADABLE:
		case SVN_ERR_AUTHZ_PARTIALLY_READABLE:
		case SVN_ERR_AUTHZ_INVALID_CONFIG:
		case SVN_ERR_AUTHZ_UNWRITABLE:
			return gcnew SvnAuthorizationException(error);

		case SVN_ERR_SVNDIFF_INVALID_HEADER:
		case SVN_ERR_SVNDIFF_CORRUPT_WINDOW:
		case SVN_ERR_SVNDIFF_BACKWARD_VIEW:
		case SVN_ERR_SVNDIFF_INVALID_OPS:
		case SVN_ERR_SVNDIFF_UNEXPECTED_END:
		case SVN_ERR_SVNDIFF_INVALID_COMPRESSED_DATA:
		case SVN_ERR_DIFF_DATASOURCE_MODIFIED:
			return gcnew SvnDiffException(error);

		case SVN_ERR_CLIENT_VERSIONED_PATH_REQUIRED:
		case SVN_ERR_CLIENT_RA_ACCESS_REQUIRED:
		case SVN_ERR_CLIENT_BAD_REVISION:
		case SVN_ERR_CLIENT_DUPLICATE_COMMIT_URL:
		case SVN_ERR_CLIENT_IS_BINARY_FILE:
		case SVN_ERR_CLIENT_INVALID_EXTERNALS_DESCRIPTION:
		case SVN_ERR_CLIENT_MODIFIED:
		case SVN_ERR_CLIENT_IS_DIRECTORY:
		case SVN_ERR_CLIENT_REVISION_RANGE:
		case SVN_ERR_CLIENT_INVALID_RELOCATION:
		case SVN_ERR_CLIENT_REVISION_AUTHOR_CONTAINS_NEWLINE:
		case SVN_ERR_CLIENT_PROPERTY_NAME:
		case SVN_ERR_CLIENT_UNRELATED_RESOURCES:
		case SVN_ERR_CLIENT_MISSING_LOCK_TOKEN:
		case SVN_ERR_CLIENT_MULTIPLE_SOURCES_DISALLOWED:
		case SVN_ERR_CLIENT_NO_VERSIONED_PARENT:
			return gcnew SvnClientApiException(error);

		
		case SVN_ERR_MERGE_INFO_PARSE_ERROR:
			return gcnew SvnException(error);

		case SVN_ERR_CANCELLED:
			if(error->message && !strncmp(MANAGED_EXCEPTION_PREFIX, error->message, prefixLength))
			{
				try
				{
					void *container = nullptr;
					sscanf(error->message + prefixLength, "%p", &container);
					
					return SvnExceptionContainer::Fetch((SvnExceptionContainer*)container);
				}
				catch(Exception^)
				{}
			}
			return gcnew SvnOperationCanceledException(error);

		case SVN_ERR_CEASE_INVOCATION:
			return gcnew SvnOperationCompletedException(error);

		default:
			if(APR_STATUS_IS_EACCES(error->apr_err))
				return gcnew SvnAuthorizationException(error);
			else if(APR_STATUS_IS_ENOSPC(error->apr_err))
				return gcnew SvnDiskFullException(error);
			else
				return gcnew SvnException(error);
		}
	}
	finally
	{
		if(clearError)
			svn_error_clear(error);
	}
}