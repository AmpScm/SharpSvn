#include "stdafx.h"

#include <apr_version.h>

using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;

//
// General Information about an assembly is controlled through the following
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
//
[assembly:AssemblyTitleAttribute("SharpSvn - Subversion for .Net 2.0")];
[assembly:AssemblyDescriptionAttribute("")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute("QQn")];
[assembly:AssemblyProductAttribute("SharpSvn")];
[assembly:AssemblyCopyrightAttribute("Copyright © QQn Project 2007")];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];

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

[assembly:AssemblyVersionAttribute("1.5.*")];

[assembly:ComVisible(false)];

[assembly:CLSCompliantAttribute(true)];

[assembly:SecurityPermission(SecurityAction::RequestMinimum, UnmanagedCode = true)];

#pragma comment(lib, "ole32.lib")

#pragma comment(lib, "libsvn_client-1.lib")
#pragma comment(lib, "libsvn_delta-1.lib")
#pragma comment(lib, "libsvn_diff-1.lib")
#pragma comment(lib, "libsvn_fs-1.lib")
#pragma comment(lib, "libsvn_fs_base-1.lib")
#pragma comment(lib, "libsvn_fs_fs-1.lib")
#if (SVN_VER_MAJOR > 1 || SVN_VER_MINOR >= 5)
#pragma comment(lib, "libsvn_fs_util-1.lib")
#pragma comment(lib, "sqlite3.lib")
#endif

#pragma comment(lib, "libsvn_ra-1.lib")
#if (SVN_VER_MAJOR > 1 || SVN_VER_MINOR >= 5)
#pragma comment(lib, "libsvn_ra_neon-1.lib")
#pragma comment(lib, "libsvn_ra_serf-1.lib")
#else
#pragma comment(lib, "libsvn_ra_dav-1.lib")
#endif
#pragma comment(lib, "libsvn_ra_local-1.lib")
#pragma comment(lib, "libsvn_ra_svn-1.lib")
#pragma comment(lib, "libsvn_repos-1.lib")
#pragma comment(lib, "libsvn_subr-1.lib")
#pragma comment(lib, "libsvn_wc-1.lib")
#pragma comment(lib, "xml.lib")
#pragma comment(lib, "libneon.lib")
#if (SVN_VER_MAJOR > 1 || SVN_VER_MINOR >= 5)
#pragma comment(lib, "serf.lib")
#endif
#pragma comment(lib, "zlibstat.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libdb44.lib")

// When statically linking neon/OpenSSL
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "User32.lib")


#if (APR_MAJOR_VERSION == 0) && (APR_MINOR_VERSION == 9)
#pragma comment(lib, "apr.lib")
#pragma comment(lib, "apriconv.lib")
#pragma comment(lib, "aprutil.lib")
#elif (APR_MAJOR_VERSION >= 1)
#pragma comment(lib, "apr-1.lib")
#pragma comment(lib, "apriconv-1.lib")
#pragma comment(lib, "aprutil-1.lib")
#else
#error Only apr 0.9.* and 1.* are supported at this time
#endif
