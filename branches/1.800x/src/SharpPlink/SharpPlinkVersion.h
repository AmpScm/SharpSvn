#pragma once


#define PLINK_FILE_DESCRIPTION SharpPlink - Putty Plink variant

#define PLINK_VER_MAJOR 0
#define PLINK_VER_MINOR 64
#define PLINK_VER_PATCH 0

#define PLINK_VER_REVISION 0
#define PLINK_VER_NUMTAG     "(SharpSvn)"
#define PLINK_VER_TAG        "-SharpSvn"
#define PLINK_FILE_NAME SharpPlink.exe

#ifndef STRINGIFY
/** Properly quote a value as a string in the C preprocessor */
#define STRINGIFY(n) STRINGIFY_HELPER(n)
/** Helper macro for STRINGIFY */
#define STRINGIFY_HELPER(n) #n
#endif


#define PLINK_VER_NUM        STRINGIFY(PLINK_VER_MAJOR) \
                           "." STRINGIFY(PLINK_VER_MINOR) \
                           "." STRINGIFY(PLINK_VER_PATCH)

/** Version number with tag (contains no whitespace) */
#define PLINK_VER_NUMBER     PLINK_VER_NUM PLINK_VER_NUMTAG

#define PLINK_VERSION        PLINK_VER_NUMBER PLINK_VER_TAG
