/* compat_win.h - Win32 type/macro shims for non-Windows builds */
#ifndef COMPAT_WIN_H
#define COMPAT_WIN_H

#ifndef _WIN32

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

/* Calling convention no-ops */
#define __cdecl
#define __fastcall
#define __forceinline static inline
#define FAR
#define IN
#define OUT
#define WINAPI
#define APIENTRY
#define CALLBACK
#define _cdecl
#define __stdcall

/* Basic types */
typedef uint8_t   BYTE;
typedef uint16_t  WORD;
typedef uint32_t  DWORD;
typedef uint64_t  QWORD;
typedef int       BOOL;
typedef char      CHAR;
typedef uint16_t  WCHAR;
typedef char     *LPSTR;
typedef const char *LPCSTR;
typedef void     *LPVOID;
typedef void     *HANDLE;
typedef void     *HWND;
typedef void     *HDC;
typedef void     *HBITMAP;
typedef void     *HMENU;
typedef void     *HINSTANCE;
typedef void     *HICON;
typedef void     *HCURSOR;
typedef uint32_t  ULONG;
typedef int32_t   LONG;
typedef int64_t   LONGLONG;
typedef uint64_t  ULONGLONG;
typedef int16_t   SHORT;
typedef uint16_t  USHORT;
typedef int       INT;
typedef unsigned int UINT;
typedef float     FLOAT;
typedef double    DOUBLE;
typedef uintptr_t DWORD_PTR;
typedef uintptr_t ULONG_PTR;

typedef union _LARGE_INTEGER {
    struct { DWORD LowPart; LONG HighPart; };
    LONGLONG QuadPart;
} LARGE_INTEGER;

typedef union _ULARGE_INTEGER {
    struct { DWORD LowPart; DWORD HighPart; };
    ULONGLONG QuadPart;
} ULARGE_INTEGER;

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* Near-memory copy/set aliases */
#define _fmemcpy  memcpy
#define _fmemset  memset

/* POSIX I/O aliases */
#define _read   read
#define _write  write
#define _open   open
#define _close  close
#define _lseek  lseek

#endif /* !_WIN32 */
#endif /* COMPAT_WIN_H */
