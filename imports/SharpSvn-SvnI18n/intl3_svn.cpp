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

#include <stdio.h>
#include <stdlib.h> // The header containing abort()

#define SHARPSVN_NO_ABORT
#include "libintl.h"

sharpsvn_dgettext_t* sharpsvn_dgettext = NULL;
sharpsvn_abort_t* sharpsvn_abort = NULL;
sharpsvn_sharpsvn_check_bdb_availability_t* sharpsvn_sharpsvn_check_bdb_availability = NULL;
sharpsvn_retry_loop_t* do_sharpsvn_retry = NULL;

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