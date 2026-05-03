/* compat_win.h - Win32 type/macro shims for non-Windows builds */
#ifndef COMPAT_WIN_H
#define COMPAT_WIN_H

#ifndef _WIN32

#include <stdint.h>
#include <stdio.h>
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

/* Message/pointer-sized types */
typedef uintptr_t WPARAM;
typedef intptr_t  LPARAM;
typedef intptr_t  LRESULT;
typedef intptr_t  LONG_PTR;
typedef uintptr_t UINT_PTR;

/* COM error type */
typedef int32_t   HRESULT;
#define S_OK         ((HRESULT)0)
#define S_FALSE      ((HRESULT)1)
#define E_FAIL       ((HRESULT)0x80004005)
#define E_INVALIDARG ((HRESULT)0x80070057)
#define E_NOTIMPL    ((HRESULT)0x80004001)
#define SUCCEEDED(hr) ((HRESULT)(hr) >= 0)
#define FAILED(hr)    ((HRESULT)(hr) < 0)

/* Function pointer / string types */
typedef void     *FARPROC;
typedef const char *PCSTR;
typedef const uint16_t *PCWSTR;
typedef uint16_t *PWSTR;
typedef BYTE     *PBYTE;
typedef DWORD    *LPDWORD;

/* MSVC 64-bit integer compat */
typedef int64_t  __int64;

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

/* GDI geometry types */
typedef struct tagPOINT { LONG x; LONG y; } POINT, *PPOINT, *LPPOINT;
typedef struct tagRECT  { LONG left; LONG top; LONG right; LONG bottom; } RECT, *PRECT, *LPRECT;
typedef struct tagSIZE  { LONG cx; LONG cy; } SIZE, *PSIZE;

/* GDI color/bitmap types */
typedef struct tagRGBQUAD {
    BYTE rgbBlue; BYTE rgbGreen; BYTE rgbRed; BYTE rgbReserved;
} RGBQUAD;
typedef struct tagBITMAPINFOHEADER {
    DWORD biSize; LONG biWidth; LONG biHeight;
    WORD  biPlanes; WORD biBitCount;
    DWORD biCompression; DWORD biSizeImage;
    LONG  biXPelsPerMeter; LONG biYPelsPerMeter;
    DWORD biClrUsed; DWORD biClrImportant;
} BITMAPINFOHEADER;
typedef void *HPALETTE;

/* DirectDraw stub (forward declaration for pointer use in gemtypes.h) */
typedef struct IDirectDrawSurface IDirectDrawSurface;

/* Multimedia result / audio types (stub structs for INST storage) */
typedef UINT MMRESULT;
#define MMSYSERR_NOERROR 0
typedef struct tagWAVEOUTCAPS {
    WORD  wMid; WORD wPid;
    DWORD vDriverVersion;
    char  szPname[32];
    DWORD dwFormats;
    WORD  wChannels; WORD wReserved1;
    DWORD dwSupport;
} WAVEOUTCAPS;
#define WHDR_DONE     0x00000001u
#define WHDR_PREPARED 0x00000002u
typedef struct tagWAVEHDR {
    char  *lpData;
    DWORD  dwBufferLength;
    DWORD  dwBytesRecorded;
    DWORD_PTR dwUser;
    DWORD  dwFlags;
    DWORD  dwLoops;
    struct tagWAVEHDR *lpNext;
    DWORD_PTR reserved;
} WAVEHDR;

/* Joystick types (stub structs for INST storage) */
#define JOY_RETURNALL 0x000000FFul
typedef struct joyinfoex_tag {
    DWORD dwSize; DWORD dwFlags;
    DWORD dwXpos; DWORD dwYpos; DWORD dwZpos;
    DWORD dwRpos; DWORD dwUpos; DWORD dwVpos;
    DWORD dwButtons; DWORD dwButtonNumber;
    DWORD dwPOV; DWORD dwReserved1; DWORD dwReserved2;
} JOYINFOEX;
typedef struct tagJOYCAPS {
    WORD wMid; WORD wPid;
    char szPname[32];
    UINT wXmin; UINT wXmax; UINT wYmin; UINT wYmax;
    UINT wZmin; UINT wZmax; UINT wNumButtons;
    UINT wPeriodMin; UINT wPeriodMax;
    UINT wRmin; UINT wRmax; UINT wUmin; UINT wUmax;
    UINT wVmin; UINT wVmax;
    UINT wCaps; UINT wMaxAxes; UINT wNumAxes; UINT wMaxButtons;
    char szRegKey[32];
    char szOEMVxD[260];
} JOYCAPS;

/* System info stub */
typedef struct _SYSTEM_INFO {
    DWORD     dwOemId;
    DWORD     dwPageSize;
    void     *lpMinimumApplicationAddress;
    void     *lpMaximumApplicationAddress;
    DWORD_PTR dwActiveProcessorMask;
    DWORD     dwNumberOfProcessors;
    DWORD     dwProcessorType;
    DWORD     dwAllocationGranularity;
    WORD      wProcessorLevel;
    WORD      wProcessorRevision;
} SYSTEM_INFO;

/* Disk geometry stub (for blockdev.h GetDiskGeometry) */
typedef struct _DISK_GEOMETRY {
    LONGLONG Cylinders;
    DWORD    MediaType;
    DWORD    TracksPerCylinder;
    DWORD    SectorsPerTrack;
    DWORD    BytesPerSector;
} DISK_GEOMETRY, *PDISK_GEOMETRY;

/* MessageBox flags and stub (used in #ifndef NDEBUG _gem_assert in gemtypes.h) */
#define MB_OK                  0x00000000u
#define MB_ABORTRETRYIGNORE    0x00000002u
#define MB_APPLMODAL           0x00000000u
#define MB_ICONERROR           0x00000010u
#define MB_ICONINFORMATION     0x00000040u
static inline int MessageBox(void *hwnd, const char *text, const char *caption, unsigned flags)
    { (void)hwnd; (void)flags; fprintf(stderr, "%s: %s\n", caption, text); return 3; } // PHASE3:
static inline void *GetFocus(void) { return NULL; }
#define wsprintf sprintf
#ifndef __debugbreak
#define __debugbreak() __builtin_trap()
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
