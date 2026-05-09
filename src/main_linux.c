#ifndef _WIN32

#include <SDL2/SDL.h>
#include "gemtypes.h"
#include "atari800.h"

void UninitThreads(void);

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
            if (e.type == SDL_QUIT)
                vi.fQuitting = TRUE;
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
