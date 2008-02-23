/* $Id$ */
/* SharpSvn gettext lookalike to allow Subversion to think it uses gettext */

#pragma once

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

#ifdef __cplusplus
}
#endif
