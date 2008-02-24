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

char * bindtextdomain (const char * domainname, const char * dirname);

typedef void *sharpsvn_abort_t();
extern sharpsvn_abort_t* sharpsvn_abort;

#ifndef SHARPSVN_NO_ABORT
/*// The header containing abort() */
#include <stdlib.h> 


#ifdef abort
/* Might be defined by some kind of debugging framework */
# undef abort
#endif

static __forceinline void __cdecl abort()
{
	if(sharpsvn_abort)
		(*sharpsvn_abort)(); // Allow throwing some kind of non-terminating exception

	abort(); // Raise the original abort method
}
#endif


#ifdef __cplusplus
}
#endif
