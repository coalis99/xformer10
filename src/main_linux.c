#ifndef _WIN32

#include <SDL2/SDL.h>
#include "gemtypes.h"
#include "atari800.h"

void UninitThreads(void);
extern const int sdl_to_vk[];

static LPARAM make_key_lparam(int scancode, int is_up)
{
    DWORD oem = (DWORD)(unsigned)scancode;
    if (is_up)
        return (LPARAM)((oem << 16) | 0xC0000001u);
    return (LPARAM)((oem << 16) | 1u);
}

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    InitProperties();
    LoadProperties(NULL, TRUE);

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
        int iVM = AddVM(0, FALSE, FALSE);
        if (iVM >= 0) {
            FInitVM(iVM);
            ColdStart(iVM);
            SelectInstance(iVM);
        }
    }

    InitSound();
    InitThreads();

    vi.fExecuting = TRUE;

    SDL_Event e;
    while (!vi.fQuitting) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                vi.fQuitting = TRUE;
            } else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                int sc = (int)e.key.keysym.scancode;
                int vk = (sc >= 0 && sc < 512) ? sdl_to_vk[sc] : 0;
                if (vk && v.cVM > 0)
                    FWinMsgVM(v.iVM, vi.hWnd,
                              e.type == SDL_KEYDOWN ? WM_KEYDOWN : WM_KEYUP,
                              (WPARAM)vk,
                              make_key_lparam(sc, e.type == SDL_KEYUP));
            }
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
