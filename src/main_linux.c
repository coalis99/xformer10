#ifndef _WIN32

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <glob.h>
#include <limits.h>
#include <SDL2/SDL.h>
#include "gemtypes.h"
#include "atari800.h"
#include "res/resource.h"
#include "menu_sdl.h"

void UninitThreads(void);
void LinuxDoCommand(int idm);
extern const int sdl_to_vk[];

static void sigint_handler(int s) { (void)s; vi.fQuitting = TRUE; }

static LPARAM make_key_lparam(int sdl_sc, int is_up)
{
    extern const unsigned char sdl_to_ps2[];
    DWORD oem = (sdl_sc >= 0 && sdl_sc < 512) ? sdl_to_ps2[sdl_sc] : 0;
    if (is_up)
        return (LPARAM)((oem << 16) | 0xC0000001u);
    return (LPARAM)((oem << 16) | 1u);
}

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    InitProperties();
    LoadProperties(NULL, TRUE);

    vi.szAppName = "Xformer";
    vi.szTitle   = "Xformer";

    rgpvm = malloc(128 * (sizeof(VM) + sizeof(VMINST)));
    cpvm = 128;

    sMaxTiles = 2;

    InitDrawing(X8, Y8, 8, NULL, FALSE);

    if (!CreateNewBitmaps()) {
        fprintf(stderr, "CreateNewBitmaps failed\n");
        SDL_Quit();
        return 1;
    }

    if (v.cVM == 0) {
        int iVM = AddVM(1, FALSE, FALSE);
        if (iVM >= 0) {
            FInitVM(iVM);
            ColdStart(iVM);
            SelectInstance(iVM);
        }
    }

    setenv("SDL_AUDIODRIVER", "pulseaudio", 1);
    {
        glob_t gl;
        if (glob("/run/user/*/pulse/native", GLOB_NOSORT, NULL, &gl) == 0
                && gl.gl_pathc > 0) {
            char buf[PATH_MAX];
            snprintf(buf, sizeof buf, "unix:%s", gl.gl_pathv[0]);
            setenv("PULSE_SERVER", buf, 1);
        }
        globfree(&gl);
    }

    InitSound();
    InitThreads();

    vi.fExecuting = TRUE;

    signal(SIGINT, sigint_handler);

    SDL_Event e;
    while (!vi.fQuitting) {
        while (SDL_PollEvent(&e)) {
            if (MenuHandleEvent(&e)) continue;
            if (e.type == SDL_QUIT) {
                vi.fQuitting = TRUE;
            } else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                int sc = (int)e.key.keysym.scancode;
                int vk = (sc >= 0 && sc < 512) ? sdl_to_vk[sc] : 0;
                if (vk && v.cVM > 0) {
                    int is_down = (e.type == SDL_KEYDOWN);
                    SDL_Keymod mod = SDL_GetModState();
                    LPARAM lp = make_key_lparam(sc, !is_down);
                    switch (sc) {
                    case SDL_SCANCODE_UP:    case SDL_SCANCODE_DOWN:
                    case SDL_SCANCODE_LEFT:  case SDL_SCANCODE_RIGHT:
                    case SDL_SCANCODE_HOME:  case SDL_SCANCODE_END:
                    case SDL_SCANCODE_INSERT: case SDL_SCANCODE_DELETE:
                    case SDL_SCANCODE_PAGEUP: case SDL_SCANCODE_PAGEDOWN:
                        lp |= 0x01000000;
                        break;
                    default:
                        break;
                    }
                    if (sc == SDL_SCANCODE_F4 && (mod & KMOD_ALT) && is_down) {
                        /* Alt+F4: close emulator */
                        vi.fQuitting = TRUE;
                    } else if (sc == SDL_SCANCODE_F4) {
                        /* F4 without Alt: xkey.c case 0x3e swallows bare F4,
                           so set the Win32 extended-key bit (bit 24) to make
                           the switch see 0x13E instead of 0x3E, falling to
                           default which passes scan 0x3E to CheckKey. */
                        FWinMsgVM(v.iVM, vi.hWnd,
                                  is_down ? WM_KEYDOWN : WM_KEYUP,
                                  (WPARAM)vk, lp | (LPARAM)0x01000000);
                    } else if (sc == SDL_SCANCODE_F1 && (mod & KMOD_ALT) && is_down) {
                        LinuxDoCommand(IDM_TURBO);
                    } else if (sc == SDL_SCANCODE_F10 && is_down) {
                        if (mod & KMOD_ALT)
                            LinuxDoCommand(IDM_CHANGEVM);
                        else if (mod & KMOD_SHIFT)
                            LinuxDoCommand(IDM_TOGGLEBASIC);
                        else if (mod & KMOD_CTRL)
                            ColdStart(v.iVM);
                        else
                            FWarmbootVM(v.iVM);
                    } else if (sc == SDL_SCANCODE_F12 && (mod & KMOD_ALT) && is_down) {
                        LinuxDoCommand(IDM_NTSCPAL);
                    } else if (sc == SDL_SCANCODE_F12 && (mod & KMOD_SHIFT) && is_down) {
                        LinuxDoCommand(IDM_COLORMONO);
                    } else {
                        FWinMsgVM(v.iVM, vi.hWnd,
                                  is_down ? WM_KEYDOWN : WM_KEYUP,
                                  (WPARAM)vk, lp);
                    }
                }
            }
        }
        /* Dispatch any dialog-opening command now that mouse-up has been
           processed and the compositor's implicit pointer grab is released */
        {
            int pending = MenuPendingCommand();
            if (pending) LinuxDoCommand(pending);
        }
        if (v.cVM > 0 && !vi.fQuitting) {
            SetEvent(ThreadStuff[0].hGoEvent);
            WaitForMultipleObjects(1, hDoneEvent, TRUE, INFINITE);
            RenderBitmap_SDL();
        }
    }

    UninitThreads();
    UninitDrawing(TRUE);
    UninitSound();
    SDL_Quit();
    return 0;
}

#endif /* !_WIN32 */
