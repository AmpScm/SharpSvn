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

namespace SharpSvn {
    using namespace System;
    using System::ComponentModel::DescriptionAttribute;

#define SVN_ERRDEF(x,y,z)                                                                          \
        [DescriptionAttribute(z)]                                                              \
        [SuppressMessage("Microsoft.Naming", "CA1702:CompoundWordsShouldBeCasedCorrectly")]    \
        [SuppressMessage("Microsoft.Naming", "CA1707:IdentifiersShouldNotContainUnderscores")] \
        [SuppressMessage("Microsoft.Naming", "CA1709:IdentifiersShouldBeCasedCorrectly")]      \
        x = x,

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

#define CatValue(x) (((SVN_ERR_ ## x ## _CATEGORY_START)-APR_OS_START_USERERR) / SVN_ERR_CATEGORY_SIZE)
/// <summary>Gets the error category of the SvnException</summary>
public enum class SvnErrorCategory
{
    OperatingSystem=-1,
    None=0,
    Bad = CatValue(BAD),
    Xml =CatValue(XML),
    IO = CatValue(IO),
    Stream = CatValue(STREAM),
    Node = CatValue(NODE),
    Entry = CatValue(ENTRY),
    WorkingCopy = CatValue(WC),
    FileSystem = CatValue(FS),
    Repository = CatValue(REPOS),
    RepositoryAccess = CatValue(RA),
    RepositoryAccessDav = CatValue(RA_DAV),
    RepositoryAccessLocal = CatValue(RA_LOCAL),
    SvnDiff = CatValue(SVNDIFF),
    ApacheModule = CatValue(APMOD),
    Client = CatValue(CLIENT),
    Misc = CatValue(MISC),
    CommandLine = CatValue(CL),
    RepositoryAccessSvn = CatValue(RA_SVN),
    Authentication = CatValue(AUTHN),
    Authorization = CatValue(AUTHZ),
    Diff = CatValue(DIFF),
    RepositoryAccessSerf = CatValue(RA_SERF),
    Malfunction = CatValue(MALFUNC),
};
}

#undef WSABASEERR // Enable including winsock errors
#include "SvnWindowsError.h"

#undef INET_E_INVALID_URL
#undef INET_E_NO_SESSION
#undef INET_E_CANNOT_CONNECT
#undef INET_E_RESOURCE_NOT_FOUND
#undef INET_E_OBJECT_NOT_FOUND
#undef INET_E_DATA_NOT_AVAILABLE
#undef INET_E_DOWNLOAD_FAILURE
#undef INET_E_AUTHENTICATION_REQUIRED
#undef INET_E_NO_VALID_MEDIA
#undef INET_E_CONNECTION_TIMEOUT
#undef INET_E_INVALID_REQUEST
#undef INET_E_UNKNOWN_PROTOCOL
#undef INET_E_SECURITY_PROBLEM
#undef INET_E_CANNOT_LOAD_DATA
#undef INET_E_CANNOT_INSTANTIATE_OBJECT
#undef INET_E_INVALID_CERTIFICATE
#undef INET_E_REDIRECT_FAILED
#undef INET_E_REDIRECT_TO_DIR
#undef INET_E_CANNOT_LOCK_REQUEST
#undef INET_E_USE_EXTEND_BINDING
#undef INET_E_TERMINATED_BIND
#undef INET_E_RESERVED_1
#undef INET_E_BLOCKED_REDIRECT_XSECURITYID
#undef INET_E_DOMINJECTIONVALIDATION
#undef INET_E_ERROR_FIRST
#undef INET_E_CODE_DOWNLOAD_DECLINED
#undef INET_E_RESULT_DISPATCHED
#undef INET_E_CANNOT_REPLACE_SFP_FILE
