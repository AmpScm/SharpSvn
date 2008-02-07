
#pragma once

// All required libraries SharpSvn needs to bind to the current build



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
#if (SVN_VER_MAJOR > 1 || SVN_VER_MINOR >= 5)
#pragma comment(lib, "serf.lib")
#endif

#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libdb" APR_STRINGIFY(DB_VERSION_MAJOR) APR_STRINGIFY(DB_VERSION_MINOR) ".lib")

#if (APR_MAJOR_VERSION == 0) && (APR_MINOR_VERSION == 9)
#  pragma comment(lib, "apr.lib")
#  pragma comment(lib, "aprutil.lib")
#  if (APU_HAVE_APR_ICONV != 0)
#    pragma comment(lib, "apriconv.lib")
#  endif
#elif (APR_MAJOR_VERSION == 1)
#pragma comment(lib, "apr-1.lib")
#pragma comment(lib, "aprutil-1.lib")
#  if (APU_HAVE_APR_ICONV != 0)
#    pragma comment(lib, "apriconv-1.lib")
#  endif
#else
#error Only apr 0.9.* and 1.* are supported at this time
#endif
