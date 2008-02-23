#include <stdio.h>
#include "libintl.h"

sharpsvn_dgettext_t* sharpsvn_dgettext = NULL;

char * bindtextdomain (const char * domainname, const char * dirname)
{
	return "/some/directory"; // Result is completely ignored. Should return the bound directory
}