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
#define __declspec(x)

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
typedef intptr_t (*FARPROC)(void);
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

/* INT_PTR / UINT_PTR signed/unsigned pointer-sized integer */
typedef intptr_t  INT_PTR;

/* FILETIME and SYSTEMTIME (used by mac_hfs.c and WIN32_FIND_DATA) */
typedef struct _FILETIME { DWORD dwLowDateTime; DWORD dwHighDateTime; } FILETIME;
typedef struct _SYSTEMTIME {
    WORD wYear; WORD wMonth; WORD wDayOfWeek; WORD wDay;
    WORD wHour; WORD wMinute; WORD wSecond; WORD wMilliseconds;
} SYSTEMTIME;
static inline BOOL SystemTimeToFileTime(const SYSTEMTIME *st, FILETIME *ft)
    { (void)st; if (ft) { ft->dwLowDateTime = 0; ft->dwHighDateTime = 0; } return TRUE; }
static inline BOOL FileTimeToSystemTime(const FILETIME *ft, SYSTEMTIME *st)
    { (void)ft; if (st) { st->wYear=1904; st->wMonth=1; st->wDay=1; st->wHour=0; st->wMinute=0; st->wSecond=0; st->wMilliseconds=0; st->wDayOfWeek=0; } return TRUE; }

/* Drive type constants (used by PdiOpenDisk in blockapi.c) */
#define DRIVE_UNKNOWN         0
#define DRIVE_NO_ROOT_DIR     1
#define DRIVE_REMOVABLE       2
#define DRIVE_FIXED           3
#define DRIVE_REMOTE          4
#define DRIVE_CDROM           5
#define DRIVE_RAMDISK         6
static inline UINT GetDriveType(const char *path) { (void)path; return DRIVE_UNKNOWN; } // PHASE3:

/* Dynamic library stubs (ASPI not available on Linux) */
static inline HANDLE LoadLibrary(const char *name) { (void)name; return NULL; } // PHASE3:
static inline FARPROC GetProcAddress(HANDLE h, const char *name) { (void)h; (void)name; return NULL; } // PHASE3:
static inline BOOL FreeLibrary(HANDLE h) { (void)h; return TRUE; } // PHASE3:

/* Heap compact stub */
static inline DWORD HeapCompact(HANDLE h, DWORD flags) { (void)h; (void)flags; return 0; }

/* VirtualAlloc/VirtualFree stubs (disk I/O buffers; map to malloc/free) */
#define MEM_COMMIT     0x1000
#define MEM_RESERVE    0x2000
#define MEM_DECOMMIT   0x4000
#define MEM_RELEASE    0x8000
#define PAGE_READWRITE 0x04
#define PAGE_READONLY  0x02
static inline void *VirtualAlloc(void *addr, size_t sz, DWORD type, DWORD prot)
    { (void)addr; (void)type; (void)prot; return malloc(sz); } // PHASE3:
static inline BOOL VirtualFree(void *p, size_t sz, DWORD type)
    { (void)sz; if (type & MEM_RELEASE) free(p); return TRUE; } // PHASE3:

/* SetErrorMode stub */
#define SEM_FAILCRITICALERRORS 0x0001
static inline DWORD SetErrorMode(DWORD mode) { (void)mode; return 0; } // PHASE3:

/* FSCTL constants (from winioctl.h; all disk I/O stubbed to FALSE) */
#define FSCTL_LOCK_VOLUME      0x00090018
#define FSCTL_UNLOCK_VOLUME    0x0009001C
#define FSCTL_DISMOUNT_VOLUME  0x00090020

/* MEDIA_TYPE enum value used in sectorio.c */
#define F3_720_512  6

/* DeviceIoControl stub (raw disk I/O not needed in Phase 1) */
typedef void *LPOVERLAPPED;
static inline BOOL DeviceIoControl(HANDLE h, DWORD code, void *in, DWORD inSz,
                                    void *out, DWORD outSz, DWORD *ret, LPOVERLAPPED ov)
    { (void)h; (void)code; (void)in; (void)inSz; (void)out; (void)outSz; (void)ret; (void)ov; return FALSE; } // PHASE3:
#define IOCTL_DISK_GET_DRIVE_GEOMETRY 0x00070000
#define IOCTL_DISK_GET_PARTITION_INFO  0x00074004

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

/* VOID alias (used in old Win32 headers like wnaspi32.h) */
#ifndef VOID
#define VOID void
#endif

/* Near-memory copy/set aliases */
#define _fmemcpy  memcpy
#define _fmemset  memset

/* POSIX I/O aliases and flag names */
#define _read   read
#define _write  write
#define _open   open
#define _close  close
#define _lseek  lseek
#define _O_RDONLY  O_RDONLY
#define _O_WRONLY  O_WRONLY
#define _O_RDWR    O_RDWR
#define _O_CREAT   O_CREAT
#define _O_TRUNC   O_TRUNC
#define _O_APPEND  O_APPEND
#define _O_BINARY  0
#define _O_TEXT    0

/* Old-style OpenFile constants (used by atari800.h macro remapping) */
#define OF_READ       O_RDONLY
#define OF_READWRITE  O_RDWR
#define OF_SHARE_COMPAT 0

/* HFILE: old-style file handle (int fd) */
typedef int HFILE;

/* Additional string pointer types */
typedef const char *LPCCH;
typedef const char *LPCTSTR;
typedef char       *LPTSTR;
typedef WORD       *LPWORD;

/* MessageBox icon aliases */
#ifndef MB_ICONHAND
#define MB_ICONHAND  MB_ICONERROR
#endif

/* Bit-field extraction macros */
#define LOWORD(l)      ((WORD)((DWORD_PTR)(l) & 0xFFFF))
#define HIWORD(l)      ((WORD)(((DWORD_PTR)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)      ((BYTE)((DWORD_PTR)(w) & 0xFF))
#define HIBYTE(w)      ((BYTE)(((DWORD_PTR)(w) >> 8) & 0xFF))
#define MAKELONG(lo,hi) ((LONG)(((WORD)(lo)) | (((DWORD)((WORD)(hi))) << 16)))
#define MAKEWORD(lo,hi) ((WORD)(((BYTE)(lo)) | (((WORD)((BYTE)(hi))) << 8)))
#define MAKELPARAM(lo,hi) ((LPARAM)MAKELONG(lo,hi))
#define MAKELRESULT(lo,hi) ((LRESULT)MAKELONG(lo,hi))

/* min/max (not defined by standard C — Windows defines them in <windef.h>) */
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

/* Heap allocation stubs (map to malloc/calloc/free) */
#include <stdlib.h>
#define HEAP_NO_SERIALIZE  0x00000001u
#define HEAP_ZERO_MEMORY   0x00000008u
#define HEAP_GENERATE_EXCEPTIONS 0x00000004u
static inline HANDLE GetProcessHeap(void) { return (HANDLE)1; }
static inline void *HeapAlloc(HANDLE h, DWORD flags, size_t size)
    { (void)h; return (flags & HEAP_ZERO_MEMORY) ? calloc(1, size) : malloc(size); }
static inline BOOL HeapFree(HANDLE h, DWORD flags, void *p)
    { (void)h; (void)flags; free(p); return TRUE; }
static inline void *HeapReAlloc(HANDLE h, DWORD flags, void *p, size_t size)
    { (void)h; (void)flags; return realloc(p, size); }

/* Console handle constants and stubs */
#define STD_INPUT_HANDLE  ((DWORD)-10)
#define STD_OUTPUT_HANDLE ((DWORD)-11)
#define STD_ERROR_HANDLE  ((DWORD)-12)
static inline HANDLE GetStdHandle(DWORD n) { (void)n; return (HANDLE)(intptr_t)0; }
static inline BOOL ReadConsole(HANDLE h, void *buf, DWORD n, DWORD *read, void *res)
    { (void)h; (void)buf; (void)n; (void)res; if (read) *read = 0; return FALSE; } // PHASE3:
static inline SHORT GetAsyncKeyState(int vk) { (void)vk; return 0; } // PHASE3:

/* Windows message constants */
#define WM_NULL           0x0000
#define WM_CREATE         0x0001
#define WM_DESTROY        0x0002
#define WM_SIZE           0x0005
#define WM_ACTIVATE       0x0006
#define WM_SETFOCUS       0x0007
#define WM_KILLFOCUS      0x0008
#define WM_PAINT          0x000F
#define WM_CLOSE          0x0010
#define WM_QUIT           0x0012
#define WM_TIMER          0x0113
#define WM_COMMAND        0x0111
#define WM_KEYDOWN        0x0100
#define WM_KEYUP          0x0101
#define WM_CHAR           0x0102
#define WM_SYSKEYDOWN     0x0104
#define WM_SYSKEYUP       0x0105
#define WM_MOUSEMOVE      0x0200
#define WM_LBUTTONDOWN    0x0201
#define WM_LBUTTONUP      0x0202
#define WM_RBUTTONDOWN    0x0204
#define WM_RBUTTONUP      0x0205
#define WM_USER           0x0400

/* PeekMessage/PostMessage flags */
#define PM_NOREMOVE 0x0000
#define PM_REMOVE   0x0001

/* MSG structure */
typedef struct tagMSG {
    HWND   hwnd;
    UINT   message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD  time;
    POINT  pt;
} MSG, *PMSG, *LPMSG;

/* Message queue stubs */
static inline BOOL PeekMessage(MSG *msg, HWND hwnd, UINT min, UINT max, UINT remove)
    { (void)msg; (void)hwnd; (void)min; (void)max; (void)remove; return FALSE; } // PHASE3:
static inline BOOL PostMessage(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
    { (void)hwnd; (void)msg; (void)wp; (void)lp; return FALSE; } // PHASE3:
static inline BOOL TranslateMessage(const MSG *msg)
    { (void)msg; return FALSE; }
static inline LRESULT DispatchMessage(const MSG *msg)
    { (void)msg; return 0; }
static inline void PostQuitMessage(int code) { (void)code; }

/* Virtual key codes */
#define VK_LBUTTON   0x01
#define VK_RBUTTON   0x02
#define VK_MBUTTON   0x04
#define VK_BACK      0x08
#define VK_TAB       0x09
#define VK_RETURN    0x0D
#define VK_SHIFT     0x10
#define VK_CONTROL   0x11
#define VK_MENU      0x12
#define VK_PAUSE     0x13
#define VK_CAPITAL   0x14
#define VK_ESCAPE    0x1B
#define VK_SPACE     0x20
#define VK_PRIOR     0x21
#define VK_NEXT      0x22
#define VK_END       0x23
#define VK_HOME      0x24
#define VK_LEFT      0x25
#define VK_UP        0x26
#define VK_RIGHT     0x27
#define VK_DOWN      0x28
#define VK_INSERT    0x2D
#define VK_DELETE    0x2E
#define VK_F1        0x70
#define VK_F2        0x71
#define VK_F3        0x72
#define VK_F4        0x73
#define VK_F5        0x74
#define VK_F6        0x75
#define VK_F7        0x76
#define VK_F8        0x77
#define VK_F9        0x78
#define VK_F10       0x79
#define VK_F11       0x7A
#define VK_F12       0x7B
#define VK_NUMLOCK   0x90
#define VK_SCROLL    0x91
#define VK_LSHIFT    0xA0
#define VK_RSHIFT    0xA1
#define VK_LCONTROL  0xA2
#define VK_RCONTROL  0xA3
#define VK_LMENU     0xA4
#define VK_RMENU     0xA5

/* Multimedia joystick message constants */
#define MM_JOY1MOVE       0x03A1
#define MM_JOY2MOVE       0x03A3
#define MM_JOY1BUTTONDOWN 0x03B5
#define MM_JOY1BUTTONUP   0x03B6
#define MM_JOY2BUTTONDOWN 0x03B7
#define MM_JOY2BUTTONUP   0x03B8
#define JOY_BUTTON1       0x0001
#define JOY_BUTTON2       0x0002
#define JOY_BUTTON3       0x0004
#define JOY_BUTTON4       0x0008

#endif /* !_WIN32 */
#endif /* COMPAT_WIN_H */
