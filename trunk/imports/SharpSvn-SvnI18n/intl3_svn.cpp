#include <stdio.h>
#include <stdlib.h> // The header containing abort()

#define SHARPSVN_NO_ABORT
#include "libintl.h"

sharpsvn_dgettext_t* sharpsvn_dgettext = NULL;
sharpsvn_abort_t* sharpsvn_abort = NULL;
sharpsvn_sharpsvn_check_bdb_availability_t* sharpsvn_sharpsvn_check_bdb_availability = NULL;
sharpsvn_maybe_handle_conflict_as_binary_t* sharpsvn_maybe_handle_conflict_as_binary = NULL;

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