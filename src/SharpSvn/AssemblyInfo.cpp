// Copyright 2007-2009 The SharpSvn Project
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

#include "stdafx.h"
#include "SvnLibraryAttribute.h"

#pragma warning(disable: 4635)
#pragma warning(disable: 4634) // XML document comment: cannot be applied:  Discarding XML document comment for invalid target.

#include <apr-1/apr_version.h>
#include <apr-1/apu_version.h>
#if (APU_HAVE_APR_ICONV != 0)
#include <apr-1/api_version.h>
#endif
#include <svn_version.h>
#include <openssl/opensslv.h>
#include <db.h>
#include <expat.h>
#define STRUCT_IOVEC_DEFINED // Kill compile error

#include <sasl/sasl.h>	// Generated by our import script as we can't use config.h
#include <sqlite3.h>
#define Byte ZByte
#include <zlib.h>
#undef Byte

using namespace System;
using namespace System::Reflection;
using namespace System::Resources;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;
using SharpSvn::Implementation::SvnLibraryAttribute;

#define SERF_VERSION APR_STRINGIFY(SERF_MAJOR_VERSION) \
    "." APR_STRINGIFY(SERF_MINOR_VERSION) \
    "." APR_STRINGIFY(SERF_PATCH_VERSION)

#define BDB_VERSION APR_STRINGIFY(DB_VERSION_MAJOR) "." APR_STRINGIFY(DB_VERSION_MINOR) "." APR_STRINGIFY(DB_VERSION_PATCH)

#define SASL_VERSION APR_STRINGIFY(SASL_VERSION_MAJOR) "." APR_STRINGIFY(SASL_VERSION_MINOR) "." APR_STRINGIFY(SASL_VERSION_STEP)

#define EXPAT_VERSION APR_STRINGIFY(XML_MAJOR_VERSION) "." APR_STRINGIFY(XML_MINOR_VERSION) "." APR_STRINGIFY(XML_MICRO_VERSION)
//
// General Information about an assembly is controlled through the following
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
//
[assembly:AssemblyTitleAttribute("SharpSvn - Subversion for .Net 2.0-3.5 and 4.0")];
[assembly:AssemblyDescriptionAttribute("SharpSvn (Compiled statically with subversion " SVN_VER_NUMBER
    ", apr " APR_VERSION_STRING
    ", apr-util " APU_VERSION_STRING
    ", Cyrus Sasl " SASL_VERSION
    ", eXpat " EXPAT_VERSION
    ", serf " SERF_VERSION
    ", sqlite" SQLITE_VERSION
    ", zlib " ZLIB_VERSION
    " and " OPENSSL_VERSION_TEXT
    ". If available this library also uses the optional Berkeley DB " BDB_VERSION
    " and our compilation of Putty's Plink)")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute("SharpSvn Project")];
[assembly:AssemblyProductAttribute("SharpSvn")];
[assembly:AssemblyCopyrightAttribute("Copyright (c) SharpSvn Project 2006-2014")];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];

[assembly:SvnLibrary("Subversion", SVN_VER_NUMBER)];
[assembly:SvnLibrary("Apr", APR_VERSION_STRING)];
[assembly:SvnLibrary("Apr-util", APU_VERSION_STRING)];
[assembly:SvnLibrary("Cyrus Sasl", SASL_VERSION)];
[assembly:SvnLibrary("eXpat", EXPAT_VERSION)];
[assembly:SvnLibrary("serf", SERF_VERSION)];
[assembly:SvnLibrary("SQLite", SQLITE_VERSION)];
[assembly:SvnLibrary("ZLib", ZLIB_VERSION)];
[assembly:SvnLibrary("OpenSSL", OPENSSL_VERSION_TEXT, SkipPrefix = true)];
[assembly:SvnLibrary("Berkeley DB", BDB_VERSION, DynamicallyLinked = true, Optional = true)];
[assembly:SvnLibrary("SharpPlink", "", DynamicallyLinked = true, UseSharpSvnVersion = true, Optional = true)];

//
// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version
//      Build Number
//      Revision
//
// You can specify all the value or you can default the Revision and Build Numbers
// by using the '*' as shown below:

[assembly:NeutralResourcesLanguageAttribute("en-US")];

[assembly:ComVisible(false)];

[assembly:CLSCompliantAttribute(true)];

[assembly:SecurityPermission(SecurityAction::RequestMinimum, UnmanagedCode = true)];

[assembly:RuntimeCompatibility(WrapNonExceptionThrows = true)];

#include "../../imports/release/include/sharpsvn-imports.h"
[assembly:AssemblyVersionAttribute("1.7999.*")];
