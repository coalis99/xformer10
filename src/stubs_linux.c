/* stubs_linux.c - Phase 1 external definitions for cross-TU inline functions.
   PeekBAtari and PokeBAtari are defined __forceinline (-> static inline) in
   atari800.c, so they are invisible to xvideo.c and xsio.c that call them.
   This file provides external definitions WITHOUT including atari800.h so that
   the conflicting 'static inline' declaration from that header cannot override
   the external linkage of these definitions.
   PHASE3: remove this file when LTO or refactoring removes the cross-TU issue. */

#include <stdint.h>

typedef unsigned long int ADDR;
typedef uint8_t BYTE;
typedef int BOOL;

/* External stubs — Phase 1 only, not functionally correct. */
BYTE PeekBAtari(void *candy, ADDR addr)   { (void)candy; (void)addr; return 0; }   // PHASE3:
BOOL PokeBAtari(void *candy, ADDR addr, BYTE b) { (void)candy; (void)addr; (void)b; return 0; } // PHASE3:
