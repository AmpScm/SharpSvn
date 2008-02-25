#include <stdio.h>
#include <stdlib.h> // The header containing abort()

#define SHARPSVN_NO_ABORT
#include "libintl.h"

sharpsvn_dgettext_t* sharpsvn_dgettext = NULL;
extern sharpsvn_abort_t* sharpsvn_abort = NULL;

char * bindtextdomain (const char * domainname, const char * dirname)
{
	domainname; // Unused
	dirname; // Unused

	return "/some/directory"; // Result is completely ignored. Should return the bound directory
}

void sharpsvn_real_abort(void)
{
	abort();
}