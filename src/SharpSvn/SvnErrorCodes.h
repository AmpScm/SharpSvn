// $Id: SvnException.h 944 2008-12-12 10:20:50Z rhuijben $
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

namespace SharpSvn {

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

#include "SvnWindowsError.h"
