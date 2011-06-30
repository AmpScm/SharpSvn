// $Id$
//
// Copyright 2008-2009 The SharpSvn Project
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

#ifndef SVN_VER_MAJOR
#error Needs SVN_VER_MAJOR
#endif

#ifndef APR_MAJOR_VERSION
#error Needs APR_MAJOR_VERSION
#endif

#ifndef SERF_MAJOR_VERSION
#error Needs SERF_MAJOR_VERSION
#endif

#ifndef DB_VERSION_MAJOR
#error Needs DB_VERSION_MAJOR
#endif

// All required libraries SharpSvn needs to bind to the current build

#pragma comment(lib, "intl3_svn.lib")

#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")

#pragma comment(lib, "libsvn_client-1.lib")
#pragma comment(lib, "libsvn_delta-1.lib")
#pragma comment(lib, "libsvn_diff-1.lib")
#pragma comment(lib, "libsvn_fs-1.lib")
#pragma comment(lib, "libsvn_fs_base-1.lib")
#pragma comment(lib, "libsvn_fs_fs-1.lib")
#if (SVN_VER_MAJOR > 1 || SVN_VER_MINOR >= 5)
#pragma comment(lib, "libsvn_fs_util-1.lib")
#endif

#pragma comment(lib, "libsvn_ra-1.lib")
#if (SVN_VER_MAJOR > 1 || SVN_VER_MINOR >= 5)
#pragma comment(lib, "libsvn_ra_neon-1.lib")
#pragma comment(lib, "libsvn_ra_serf-1.lib")
#pragma comment(lib, "libsasl.lib")
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
#if (SERF_MAJOR_VERSION == 0)
#pragma comment(lib, "serf.lib")
#else
#pragma comment(lib, "serf-1.lib")
#endif

#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "secur32.lib")
#pragma comment(lib, "libdb" APR_STRINGIFY(DB_VERSION_MAJOR) APR_STRINGIFY(DB_VERSION_MINOR) ".lib")

#if (SVN_VER_MAJOR > 1 || SVN_VER_MINOR >= 6)
#pragma comment(lib, "sqlite3.lib")
#endif

#if (APR_MAJOR_VERSION == 0) && (APR_MINOR_VERSION == 9)
#  pragma comment(lib, "apr.lib")
#  pragma comment(lib, "aprutil.lib")
#elif (APR_MAJOR_VERSION == 1)
#  pragma comment(lib, "apr-1.lib")
#  pragma comment(lib, "aprutil-1.lib")
#else
#  error Only apr 0.9.* and 1.* are supported at this time
#endif
