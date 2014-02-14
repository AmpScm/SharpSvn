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
#include "GitLibraryAttribute.h"

#pragma warning(disable: 4635)
#pragma warning(disable: 4634) // XML document comment: cannot be applied:  Discarding XML document comment for invalid target.

#include <apr-1/apr_version.h>
#include <svn_version.h>

using namespace System;
using namespace System::Reflection;
using namespace System::Resources;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;
using SharpGit::Implementation::GitLibraryAttribute;

// General Information about an assembly is controlled through the following
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
//
[assembly:AssemblyTitleAttribute("SharpGit - Git for .Net 2.0-3.5 and 4.0")];
[assembly:AssemblyDescriptionAttribute("SharpGit (Compiled statically with libgit2 " LIBGIT2_VERSION
									   ", apr " APR_VERSION_STRING
									   " and the platform and diff libraries of Subversion " SVN_VER_NUMBER
									   ". if available this library also uses our compilation of Putty's Plink)")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute("SharpSvn Project")];
[assembly:AssemblyProductAttribute("SharpSvn")];
[assembly:AssemblyCopyrightAttribute("Copyright (c) SharpSvn Project 2013")];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];

[assembly:GitLibrary("Libgit2", LIBGIT2_VERSION)]
[assembly:GitLibrary("Apr", APR_VERSION_STRING)];
[assembly:GitLibrary("Subversion", SVN_VER_NUMBER)];
[assembly:GitLibrary("SharpPlink", "", DynamicallyLinked=true, UseSharpGitVersion=true, Optional=true)];

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
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "User32.lib")

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Version.lib")

//#include "../../imports/release/include/sharpsvn-imports.h"
[assembly:AssemblyVersionAttribute("0.2000.*")]; 

#pragma comment(lib, "intl3_svn.lib")
#pragma comment(lib, "xml.lib")
#pragma comment(lib, "libsvn_subr-1.lib")
#pragma comment(lib, "sqlite3.lib")


#if (APR_MAJOR_VERSION == 0)
#  pragma comment(lib, "apr.lib")
#  pragma comment(lib, "aprutil.lib")
#elif (APR_MAJOR_VERSION == 1)
#  pragma comment(lib, "apr-1.lib")
#  pragma comment(lib, "aprutil-1.lib")
#else
#  error Only apr 0.9.* and 1.* are supported at this time
#endif

