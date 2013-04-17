
// sbs_pdb.h
// 06-25-2006 Sven B. Schreiber
// sbs@orgon.com

#pragma once
#pragma warning(push)
#pragma warning(disable: 4200)

// =================================================================
// CONSTANTS
// =================================================================

#define PDB_UNUSED_16       (( WORD) -1) // unused stream (16-bit)
#define PDB_UNUSED_32       ((DWORD) -1) // unused stream (32-bit)

#define PDB_TI_MIN          0x00001000   // type index base
#define PDB_TI_MAX          0x00FFFFFF   // type index limit

// =================================================================
// STREAM IDS
// =================================================================

#define PDB_STREAM_ROOT     0 // PDB root directory
#define PDB_STREAM_PDB      1 // PDB stream info
#define PDB_STREAM_TPI      2 // type info
#define PDB_STREAM_DBI      3 // debug info

// =================================================================
// PDB 2.00 STRUCTURES
// =================================================================

#define PDB_VERSION_200     200    // binary version number
#define PDB_SIGNATURE_200_  0x2C   // signature size (bytes)
#define PDB_SIGNATURE_200 \
        "Microsoft C/C++ program database 2.00\r\n\032JG\0"

// -----------------------------------------------------------------

typedef struct _PDB_STREAM_200
    {
    DWORD dStreamBytes;            // stream size (-1 = unused)
    PVOID pReserved;               // implementation dependent
    }
    PDB_STREAM_200, *PPDB_STREAM_200, **PPPDB_STREAM_200;

#define PDB_STREAM_200_ sizeof (PDB_STREAM_200)

// -----------------------------------------------------------------

typedef struct _PDB_HEADER_200
    {
    BYTE           abSignature [PDB_SIGNATURE_200_]; // version ID
    DWORD          dPageBytes;     // 0x0400, 0x0800, 0x1000
    WORD           wStartPage;     // 0x0009, 0x0005, 0x0002
    WORD           wFilePages;     // file size / dPageBytes
    PDB_STREAM_200 RootStream;     // stream directory
    WORD           awRootPages []; // pages containing PDB_ROOT_200
    }
    PDB_HEADER_200, *PPDB_HEADER_200, **PPPDB_HEADER_200;

#define PDB_HEADER_200_ sizeof (PDB_HEADER_200)

#define PDB_HEADER_200__(_n) \
        (PDB_HEADER_200_ + ((DWORD) (_n) * WORD_))

// -----------------------------------------------------------------

typedef struct _PDB_ROOT_200
    {
    WORD           wStreams;       // number of streams
    WORD           wReserved;      // not used
    PDB_STREAM_200 aStreams [];    // stream size list
    }
    PDB_ROOT_200, *PPDB_ROOT_200, **PPPDB_ROOT_200;

#define PDB_ROOT_200_ sizeof (PDB_ROOT_200)

#define PDB_ROOT_200__(_n) \
        (PDB_ROOT_200_ + ((DWORD) (_n) * PDB_STREAM_200_))

// =================================================================
// PDB 7.00 STRUCTURES
// =================================================================

#define PDB_VERSION_700     700    // binary version number
#define PDB_SIGNATURE_700_  0x20   // signature size (bytes)
#define PDB_SIGNATURE_700 \
        "Microsoft C/C++ MSF 7.00\r\n\032DS\0\0"

// -----------------------------------------------------------------

typedef struct _PDB_HEADER_700
    {
    BYTE  abSignature [PDB_SIGNATURE_700_]; // version ID
    DWORD dPageBytes;              // 0x0400
    DWORD dFlagPage;               // 0x0002
    DWORD dFilePages;              // file size / dPageBytes
    DWORD dRootBytes;              // stream directory size
    DWORD dReserved;               // 0
    DWORD adIndexPages [];         // root page index pages
    }
    PDB_HEADER_700, *PPDB_HEADER_700, **PPPDB_HEADER_700;

#define PDB_HEADER_700_ sizeof (PDB_HEADER_700)

#define PDB_HEADER_700__(_n) \
        (PDB_HEADER_700_ + ((DWORD) (_n) * DWORD_))

// -----------------------------------------------------------------

typedef struct _PDB_ROOT_700
    {
    DWORD dStreams;                // number of streams
    DWORD adStreamBytes [];        // stream size list
    }
    PDB_ROOT_700, *PPDB_ROOT_700, **PPPDB_ROOT_700;

#define PDB_ROOT_700_ sizeof (PDB_ROOT_700)

#define PDB_ROOT_700__(_n) \
        (PDB_ROOT_700_ + ((DWORD) (_n) * DWORD_))

// =================================================================
// VERSION-INDEPENDENT PDB STRUCTURES
// =================================================================

typedef struct _PDB_VERSION
    {
    DWORD dVersion;                // version number
    DWORD dHeader;                 // header size
    PBYTE pbSignature;             // version ID
    }
    PDB_VERSION, *PPDB_VERSION, **PPPDB_VERSION;

#define PDB_VERSION_ sizeof (PDB_VERSION)

// -----------------------------------------------------------------

typedef union _PDB_HEADER
    {
    BYTE           abSignature []; // version ID
    PDB_HEADER_200 V200;           // version 2.00 header
    PDB_HEADER_700 V700;           // version 7.00 header
    }
    PDB_HEADER, *PPDB_HEADER, **PPPDB_HEADER;

#define PDB_HEADER_ sizeof (PDB_HEADER)

// -----------------------------------------------------------------

typedef union _PDB_ROOT
    {
    PDB_ROOT_200 V200;             // version 2.00 root directory
    PDB_ROOT_700 V700;             // version 7.00 root directory
    }
    PDB_ROOT, *PPDB_ROOT, **PPPDB_ROOT;

#define PDB_ROOT_ sizeof (PDB_ROOT)

// -----------------------------------------------------------------

typedef struct _PDB_FILE
    {
    WORD        awPath [MAX_PATH]; // fully qualified path
    PPDB_HEADER pHeader;           // header
    PPDB_ROOT   pRoot;             // root directory
    DWORD       dRoot;             // root directory size
    DWORD       dVersion;          // PDB version number
    DWORD       dStreams;          // number of streams
    }
    PDB_FILE, *PPDB_FILE, **PPPDB_FILE;

#define PDB_FILE_ sizeof (PDB_FILE)

// -----------------------------------------------------------------

typedef struct _PDB_STREAM
    {
    PVOID pData;                   // stream data pointer
    DWORD dData;                   // stream size in bytes
    BOOL  fUnused;                 // indicates unused stream
    }
    PDB_STREAM, *PPDB_STREAM, **PPPDB_STREAM;

#define PDB_STREAM_ sizeof (PDB_STREAM)

// -----------------------------------------------------------------

typedef struct _PDB_DATA
    {
    WORD       awPath [MAX_PATH];  // fully qualified path
    DWORD      dVersion;           // PDB version number
    DWORD      dStreams;           // number of streams
    PDB_STREAM aStreams [];
    }
    PDB_DATA, *PPDB_DATA, **PPPDB_DATA;

#define PDB_DATA_ sizeof (PDB_DATA)
#define PDB_DATA__(_n) (PDB_DATA_ + ((DWORD) (_n) * PDB_STREAM_))

#pragma warning(pop)