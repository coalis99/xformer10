/****************************************************************************

    MENU_SDL.C

    - SDL2 menu bar for xformer10 on Linux/Pi

    Copyright (C) 1991-2021 by Darek Mihocka. All Rights Reserved.
    Branch Always Software. http://www.emulators.com/

    This file is part of the Xformer project and subject to the MIT license terms
    in the LICENSE file found in the top-level directory of this distribution.
    No part of Xformer, including this file, may be copied, modified, propagated,
    or distributed except according to the terms contained in the LICENSE file.

****************************************************************************/

#ifndef _WIN32

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "menu_sdl.h"

#define FONT_PATH  "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
#define FONT_SIZE  14
#define NUM_TOPS   4

static const char *gTopLabels[NUM_TOPS] = { "File", "VM", "Window", "Disk" };
static const int   gTopX[NUM_TOPS]      = { 8, 60, 110, 185 };

static TTF_Font    *gFont            = NULL;
static SDL_Texture *gTopTex[NUM_TOPS];
static int          gTopW[NUM_TOPS];
static int          gTopH[NUM_TOPS];
static int          gMenuReady       = 0;

void MenuInit(SDL_Renderer *ren)
{
    if (TTF_Init() < 0) {
        fprintf(stderr, "MenuInit: TTF_Init failed: %s\n", TTF_GetError());
        return;
    }
    gFont = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if (!gFont) {
        fprintf(stderr, "MenuInit: TTF_OpenFont failed: %s\n", TTF_GetError());
        TTF_Quit();
        return;
    }

    SDL_Color fg = {230, 230, 230, 255};
    for (int i = 0; i < NUM_TOPS; i++) {
        SDL_Surface *surf = TTF_RenderUTF8_Blended(gFont, gTopLabels[i], fg);
        if (!surf) { gTopTex[i] = NULL; continue; }
        gTopTex[i] = SDL_CreateTextureFromSurface(ren, surf);
        gTopW[i]   = surf->w;
        gTopH[i]   = surf->h;
        SDL_FreeSurface(surf);
    }
    gMenuReady = 1;
}

void MenuQuit(void)
{
    if (!gMenuReady) return;
    for (int i = 0; i < NUM_TOPS; i++) {
        if (gTopTex[i]) { SDL_DestroyTexture(gTopTex[i]); gTopTex[i] = NULL; }
    }
    if (gFont) { TTF_CloseFont(gFont); gFont = NULL; }
    TTF_Quit();
    gMenuReady = 0;
}

void MenuRender(SDL_Renderer *ren)
{
    if (!gMenuReady) return;

    int winW, winH;
    SDL_GetRendererOutputSize(ren, &winW, &winH);

    /* bar background */
    SDL_SetRenderDrawColor(ren, 50, 50, 50, 255);
    SDL_Rect bar = {0, 0, winW, MENU_H};
    SDL_RenderFillRect(ren, &bar);

    /* top-level labels */
    for (int i = 0; i < NUM_TOPS; i++) {
        if (!gTopTex[i]) continue;
        int y = (MENU_H - gTopH[i]) / 2;
        SDL_Rect dst = { gTopX[i], y, gTopW[i], gTopH[i] };
        SDL_RenderCopy(ren, gTopTex[i], NULL, &dst);
    }

    /* 1-pixel separator at bottom of bar */
    SDL_SetRenderDrawColor(ren, 80, 80, 80, 255);
    SDL_RenderDrawLine(ren, 0, MENU_H - 1, winW - 1, MENU_H - 1);
}

int MenuHandleEvent(SDL_Event *e)
{
    (void)e;
    return 0;
}

#endif /* !_WIN32 */
