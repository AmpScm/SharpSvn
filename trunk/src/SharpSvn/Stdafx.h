// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#pragma warning(push)
#pragma warning(disable: 4635)
#pragma warning(disable: 4634) // XML document comment: cannot be applied:  Discarding XML document comment for invalid target. 
#include <apr-1/apr.h>
#include <apr-1/apr_file_io.h>
#include <svn_client.h>
#include <svn_time.h>
#include <svn_repos.h>

#ifndef UNUSED_ALWAYS
#define UNUSED_ALWAYS(x) (x)
#endif

#pragma warning(pop)

using System::Runtime::InteropServices::OutAttribute;