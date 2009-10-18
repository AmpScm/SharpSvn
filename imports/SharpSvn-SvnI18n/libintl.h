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

/* $Id$ */
/* SharpSvn gettext lookalike to allow Subversion to think it uses gettext */

#pragma once

#ifndef SHARPSVN_NO_ABORT
#include <stdlib.h> // The header containing abort()
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef char* sharpsvn_dgettext_t(const char* domain, const char* msgid, int category);
extern sharpsvn_dgettext_t* sharpsvn_dgettext;

/* External definitions from GNU gettext 0.16.1 manual page of FreeBSD 6.3 */
static __forceinline char* gettext(const char* msgid)
{
	if (sharpsvn_dgettext)
		return (*sharpsvn_dgettext)(NULL, msgid, -1);

	return (char*)msgid;
}

static __forceinline char* dgettext(const char* domain, const char* msgid)
{
	if (sharpsvn_dgettext)
		return (*sharpsvn_dgettext)(domain, msgid, -1);

	return (char*)msgid;
}

static __forceinline char* dcgettext(const char* domain, const char* msgid, int category)
{
	if (sharpsvn_dgettext)
		return (*sharpsvn_dgettext)(domain, msgid, category);

	return (char*)msgid;
}

static __forceinline char* dngettext(const char* domain, const char* msgid, const char* msgid_plural,
  unsigned long int n)
{
    return dgettext(domain, (n == 1) ? msgid : msgid_plural);
}

struct svn_error_t;
struct apr_pool_t;

typedef struct svn_error_t* sharpsvn_sharpsvn_check_bdb_availability_t();
extern sharpsvn_sharpsvn_check_bdb_availability_t* sharpsvn_sharpsvn_check_bdb_availability;
static __forceinline struct svn_error_t* check_bdb_availability()
{
	if (sharpsvn_sharpsvn_check_bdb_availability)
		return (*sharpsvn_sharpsvn_check_bdb_availability)();
		
	return NULL;
}

char * bindtextdomain (const char * domainname, const char * dirname);

typedef void sharpsvn_abort_t(void);
extern sharpsvn_abort_t* sharpsvn_abort;
extern void sharpsvn_real_abort(void);

typedef int sharpsvn_retry_loop_t(int n, int err, int os_err, const char *expr);
extern sharpsvn_retry_loop_t* do_sharpsvn_retry;

static __forceinline int __cdecl sharpsvn_retry(int n, int err, int os_err, const char *expr)
{
	if (do_sharpsvn_retry)
		return (*do_sharpsvn_retry)(n, err, os_err, expr);

	return 1;
}

#ifndef SHARPSVN_NO_ABORT
/*// The header containing abort() */
#include <stdlib.h> 


#ifdef abort
/* Might be defined by some kind of debugging framework */
# undef abort
#endif

#pragma warning(push)
#pragma warning(disable: 4211)
static __forceinline void __cdecl abort()
{
	if(sharpsvn_abort)
		(*sharpsvn_abort)(); // Allow throwing some kind of non-terminating exception

	sharpsvn_real_abort(); // Raise the original abort method
}

#pragma warning(pop)


#endif


#ifdef __cplusplus
}
#endif
