// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#pragma warning(push)
#pragma warning(disable: 4635)
#pragma warning(disable: 4634) // XML document comment: cannot be applied:  Discarding XML document comment for invalid target.
#include <apr-1/apr.h>
#include <apr-1/apr_strings.h>
#include <apr-1/apr_file_io.h>
#include <svn_client.h>
#include <svn_path.h>
#include <svn_time.h>
#include <svn_repos.h>
#include <svn_io.h>

#ifndef UNUSED_ALWAYS
#define UNUSED_ALWAYS(x) (x)
#endif

#pragma warning(pop)

#pragma warning(disable: 4706) // assignment within conditional expression

using System::Runtime::InteropServices::OutAttribute;
using System::Diagnostics::CodeAnalysis::SuppressMessageAttribute;