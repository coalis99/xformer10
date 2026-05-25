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
#include "gemtypes.h"
#include "menu_sdl.h"

extern void LinuxDoCommand(int idm);

#define FONT_PATH         "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
#define FONT_SIZE         14
#define NUM_TOPS          4
#define MAX_ITEMS         16
#define MENU_ITEM_H       20
#define MENU_SEP_H        8
#define MENU_PAD          8
#define MENU_SHORTCUT_GAP 16
#define MENU_DROP_MIN_W   160
#define MENU_CHECK_AREA   16  /* reserved width for checkmark left of label */

static const char *gTopLabels[NUM_TOPS] = { "File", "VM", "Window", "Disk" };
static const int   gTopX[NUM_TOPS]      = { 8, 60, 110, 185 };

static TTF_Font    *gFont         = NULL;
static SDL_Texture *gTopTex[NUM_TOPS];
static int          gTopW[NUM_TOPS];
static int          gTopH[NUM_TOPS];
static SDL_Texture *gCheckTex     = NULL;
static int          gCheckW       = 0;
static int          gCheckH       = 0;
static int          gMenuReady    = 0;
static int          gMenuOpen        = -1;
static int          gMenuHover       = -1;

typedef struct {
    const char  *label;
    const char  *shortcut;
    int          idm;       /* 0=separator, -1=always-grayed, else IDM */
    int          needsVM;   /* gray when v.iVM < 0 */
    SDL_Texture *labelTex;
    int          labelW, labelH;
    SDL_Texture *shortTex;
    int          shortW, shortH;
} MenuItem;

static MenuItem gItems[NUM_TOPS][MAX_ITEMS];
static int      gItemCount[NUM_TOPS];
static int      gDropW[NUM_TOPS];

static const struct { const char *lbl; const char *sc; int idm; int needsVM; }
kDef[NUM_TOPS][MAX_ITEMS] = {
    /* File */
    {
        {"Open Folder...",             NULL, IDM_OPENFOLDER, 0},
        {"Add Atari 800",              NULL, IDM_ADDVM1,     0},
        {"Delete VM",                  NULL, IDM_DELVM,      1},
        {NULL,                         NULL, 0,              0},
        {"New Session",                NULL, IDM_NEW,        0},
        {"Load Session...",            NULL, IDM_LOAD,       0},
        {"Save Session As...",         NULL, IDM_SAVEAS,     0},
        {NULL,                         NULL, 0,              0},
        {"Restore Last Session",       NULL, IDM_AUTOLOAD,   0},
        {NULL,                         NULL, 0,              0},
        {"Next VM",                    NULL, IDM_NEXTVM,     1},
        {"Previous VM",                NULL, IDM_PREVVM,     1},
        {NULL,                         NULL, 0,              0},
        {"Exit",                       NULL, IDM_EXIT,       0},
    },
    /* VM */
    {
        {"Warm Start",     "F10",       IDM_WARMSTART,   1},
        {"Cold Start",     "Ctrl+F10",  IDM_COLDSTART,   1},
        {"Toggle BASIC",   "Shift+F10", IDM_TOGGLEBASIC, 1},
        {"Change Type",    "Alt+F10",   IDM_CHANGEVM,    1},
        {NULL,             NULL,        0,               0},
        {"Emulate PAL",    "Alt+F12",   IDM_NTSCPAL,     1},
        {"Switch Monitor", "Shift+F12", IDM_COLORMONO,   1},
    },
    /* Window */
    {
        {"Full Screen",                "Alt+Enter", IDM_FULLSCREEN,       0},
        {"Stretch Mode",               "F12",       IDM_STRETCH,          0},
        {"Tile All VMs",               "F5",        IDM_TILE,             0},
        {NULL,                         NULL,        0,                    0},
        {"Turbo Mode",                 "Alt+F1",    IDM_TURBO,            1},
        {NULL,                         NULL,        0,                    0},
        {"Auto-detect VM Type",        NULL,        IDM_AUTOKILL,         0},
        {NULL,                         NULL,        0,                    0},
        {"Disable L-CTRL as FIRE",     NULL,        IDM_LCTRLFIRE,        0},
        {NULL,                         NULL,        0,                    0},
        {"Mouse Wheel Sensitivity High", NULL,      IDM_WHEELSENS,        0},
        {NULL,                         NULL,        0,                    0},
        {"My Video Cards Sucks",       NULL,        IDM_MYVIDEOCARDSUCKS, 0},
        {NULL,                         NULL,        0,                    0},
        {"Enable Sound",               "Alt+S",     IDM_TOGGLESOUND,      0},
    },
    /* Disk */
    {
        {"D1: Mount...",           NULL, IDM_D1,         1},
        {"D1: Unmount",            NULL, IDM_D1U,        1},
        {"D1: Write Protect",      NULL, IDM_WP1,        1},
        {"D1: Create Blank",       NULL, IDM_D1BLANKSD,  1},
        {"D1: Extract DOS Files",  NULL, IDM_IMPORTDOS1, 1},
        {NULL,                     NULL, 0,              0},
        {"D2: Mount...",           NULL, IDM_D2,         1},
        {"D2: Unmount",            NULL, IDM_D2U,        1},
        {"D2: Write Protect",      NULL, IDM_WP2,        1},
        {"D2: Create Blank",       NULL, IDM_D2BLANKSD,  1},
        {"D2: Extract DOS Files",  NULL, IDM_IMPORTDOS2, 1},
        {NULL,                     NULL, 0,              0},
        {"Cartridge...",           NULL, IDM_CART,       1},
        {"Remove Cartridge",       NULL, IDM_NOCART,     1},
    },
};

static const int kItemCount[NUM_TOPS] = {14, 7, 15, 14};

/* Returns y offset of item j within dropdown m (relative to MENU_H) */
static int ItemYOffset(int m, int j)
{
    int yoff = 0;
    for (int k = 0; k < j; k++)
        yoff += (gItems[m][k].idm == 0) ? MENU_SEP_H : MENU_ITEM_H;
    return yoff;
}

/* Returns total pixel height of dropdown m */
static int DropHeight(int m)
{
    return ItemYOffset(m, gItemCount[m]);
}

/* Returns bounding rect of open dropdown for menu m */
static SDL_Rect DropRect(int m)
{
    return (SDL_Rect){ gTopX[m], MENU_H, gDropW[m], DropHeight(m) };
}

/* Returns index of the top-level label hit at x, or -1 */
static int HitTopLabel(int x)
{
    for (int i = 0; i < NUM_TOPS; i++) {
        if (x >= gTopX[i] - 2 && x < gTopX[i] + gTopW[i] + 2)
            return i;
    }
    return -1;
}

/* Returns item index within dropdown m at pixel (x,y), or -1 */
static int HitItem(int m, int x, int y)
{
    SDL_Rect r = DropRect(m);
    if (x < r.x || x >= r.x + r.w || y < r.y || y >= r.y + r.h)
        return -1;
    int yrel = y - MENU_H;
    int yacc = 0;
    for (int j = 0; j < gItemCount[m]; j++) {
        int h = (gItems[m][j].idm == 0) ? MENU_SEP_H : MENU_ITEM_H;
        if (yrel < yacc + h)
            return j;
        yacc += h;
    }
    return -1;
}

static void DispatchMenuCmd(int idm)
{
    switch (idm) {
    case IDM_EXIT:      vi.fQuitting = TRUE;      break;
    case IDM_WARMSTART: FWarmbootVM(v.iVM);        break;
    case IDM_COLDSTART: ColdStart(v.iVM);          break;
    case IDM_D1:
    case IDM_D2:
        LinuxDoCommand(idm);
        break;
    default:            LinuxDoCommand(idm);       break;
    }
}

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

    /* Top-level label textures */
    SDL_Color fg = {230, 230, 230, 255};
    for (int i = 0; i < NUM_TOPS; i++) {
        SDL_Surface *surf = TTF_RenderUTF8_Blended(gFont, gTopLabels[i], fg);
        if (!surf) { gTopTex[i] = NULL; continue; }
        gTopTex[i] = SDL_CreateTextureFromSurface(ren, surf);
        gTopW[i]   = surf->w;
        gTopH[i]   = surf->h;
        SDL_FreeSurface(surf);
    }

    /* Checkmark texture (U+2713 ✓) */
    SDL_Color white = {255, 255, 255, 255};
    {
        SDL_Surface *surf = TTF_RenderUTF8_Blended(gFont, "\xe2\x9c\x93", white);
        if (surf) {
            gCheckTex = SDL_CreateTextureFromSurface(ren, surf);
            gCheckW   = surf->w;
            gCheckH   = surf->h;
            SDL_FreeSurface(surf);
        }
    }

    /* Item textures — white so SDL_SetTextureColorMod can dim them */
    for (int m = 0; m < NUM_TOPS; m++) {
        gItemCount[m] = kItemCount[m];
        int maxW = 0;
        for (int j = 0; j < gItemCount[m]; j++) {
            MenuItem *it = &gItems[m][j];
            it->label    = kDef[m][j].lbl;
            it->shortcut = kDef[m][j].sc;
            it->idm      = kDef[m][j].idm;
            it->needsVM  = kDef[m][j].needsVM;
            if (it->idm == 0 || !it->label) continue;

            SDL_Surface *surf = TTF_RenderUTF8_Blended(gFont, it->label, white);
            if (surf) {
                it->labelTex = SDL_CreateTextureFromSurface(ren, surf);
                it->labelW   = surf->w;
                it->labelH   = surf->h;
                SDL_FreeSurface(surf);
            }
            if (it->shortcut) {
                surf = TTF_RenderUTF8_Blended(gFont, it->shortcut, white);
                if (surf) {
                    it->shortTex = SDL_CreateTextureFromSurface(ren, surf);
                    it->shortW   = surf->w;
                    it->shortH   = surf->h;
                    SDL_FreeSurface(surf);
                }
            }
            int needed = MENU_PAD + MENU_CHECK_AREA + it->labelW
                       + (it->shortcut ? MENU_SHORTCUT_GAP + it->shortW : 0)
                       + MENU_PAD;
            if (needed > maxW) maxW = needed;
        }
        gDropW[m] = (maxW > MENU_DROP_MIN_W) ? maxW : MENU_DROP_MIN_W;
    }

    gMenuReady = 1;
}

void MenuQuit(void)
{
    if (!gMenuReady) return;
    for (int i = 0; i < NUM_TOPS; i++) {
        if (gTopTex[i]) { SDL_DestroyTexture(gTopTex[i]); gTopTex[i] = NULL; }
    }
    for (int m = 0; m < NUM_TOPS; m++) {
        for (int j = 0; j < gItemCount[m]; j++) {
            if (gItems[m][j].labelTex) {
                SDL_DestroyTexture(gItems[m][j].labelTex);
                gItems[m][j].labelTex = NULL;
            }
            if (gItems[m][j].shortTex) {
                SDL_DestroyTexture(gItems[m][j].shortTex);
                gItems[m][j].shortTex = NULL;
            }
        }
    }
    if (gCheckTex) { SDL_DestroyTexture(gCheckTex); gCheckTex = NULL; }
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

    /* separator line at bottom of bar */
    SDL_SetRenderDrawColor(ren, 80, 80, 80, 255);
    SDL_RenderDrawLine(ren, 0, MENU_H - 1, winW - 1, MENU_H - 1);

    if (gMenuOpen < 0) return;

    /* dropdown panel */
    int m = gMenuOpen;
    SDL_Rect dr = DropRect(m);

    SDL_SetRenderDrawColor(ren, 55, 55, 55, 255);
    SDL_RenderFillRect(ren, &dr);
    SDL_SetRenderDrawColor(ren, 100, 100, 100, 255);
    SDL_RenderDrawRect(ren, &dr);

    int yacc = MENU_H;
    for (int j = 0; j < gItemCount[m]; j++) {
        MenuItem *it = &gItems[m][j];
        int itemH = (it->idm == 0) ? MENU_SEP_H : MENU_ITEM_H;

        if (it->idm == 0) {
            SDL_SetRenderDrawColor(ren, 90, 90, 90, 255);
            SDL_RenderDrawLine(ren,
                dr.x + MENU_PAD,          yacc + MENU_SEP_H / 2,
                dr.x + dr.w - MENU_PAD,   yacc + MENU_SEP_H / 2);
        } else {
            int isGrayed  = (it->idm < 0) || (it->needsVM && v.iVM < 0);
            if (it->idm == IDM_D1U && v.iVM >= 0) isGrayed |= !rgpvm[v.iVM]->rgvd[0].sz[0];
            if (it->idm == IDM_D2U && v.iVM >= 0) isGrayed |= !rgpvm[v.iVM]->rgvd[1].sz[0];
            if (it->idm == IDM_WP1 && v.iVM >= 0)        isGrayed |= !rgpvm[v.iVM]->rgvd[0].sz[0];
            if (it->idm == IDM_WP2 && v.iVM >= 0)        isGrayed |= !rgpvm[v.iVM]->rgvd[1].sz[0];
            if (it->idm == IDM_IMPORTDOS1 && v.iVM >= 0) isGrayed |= !rgpvm[v.iVM]->rgvd[0].sz[0];
            if (it->idm == IDM_IMPORTDOS2 && v.iVM >= 0) isGrayed |= !rgpvm[v.iVM]->rgvd[1].sz[0];
            if (it->idm == IDM_NOCART && v.iVM >= 0)     isGrayed |= !rgpvm[v.iVM]->rgcart.fCartIn;
            if (it->idm == IDM_DELVM)  isGrayed |= (v.cVM <= 1);
            if (it->idm == IDM_NEXTVM) isGrayed |= (v.cVM <= 1);
            if (it->idm == IDM_PREVVM) isGrayed |= (v.cVM <= 1);
            int isChecked = 0;
            if (it->idm == IDM_TURBO)    isChecked = !fBrakes;
            if (it->idm == IDM_NTSCPAL)  isChecked = (v.iVM >= 0 && rgpvm[v.iVM]->fEmuPAL);
            if (it->idm == IDM_AUTOLOAD) isChecked = v.fSaveOnExit;
            if (it->idm == IDM_WP1 && v.iVM >= 0 && rgpvm[v.iVM]->rgvd[0].sz[0])
                isChecked = FWriteProtectDiskVM(v.iVM, 0, FALSE, FALSE);
            if (it->idm == IDM_WP2 && v.iVM >= 0 && rgpvm[v.iVM]->rgvd[1].sz[0])
                isChecked = FWriteProtectDiskVM(v.iVM, 1, FALSE, FALSE);

            if (gMenuHover == j && !isGrayed) {
                SDL_SetRenderDrawColor(ren, 80, 110, 160, 255);
                SDL_Rect hlr = { dr.x + 1, yacc + 1, dr.w - 2, itemH - 2 };
                SDL_RenderFillRect(ren, &hlr);
            }

            Uint8 c = isGrayed ? 100 : 230;

            if (isChecked && gCheckTex) {
                SDL_SetTextureColorMod(gCheckTex, c, c, c);
                int ty = yacc + (itemH - gCheckH) / 2;
                SDL_Rect dst = { dr.x + MENU_PAD, ty, gCheckW, gCheckH };
                SDL_RenderCopy(ren, gCheckTex, NULL, &dst);
            }
            if (it->labelTex) {
                SDL_SetTextureColorMod(it->labelTex, c, c, c);
                int ty = yacc + (itemH - it->labelH) / 2;
                SDL_Rect dst = { dr.x + MENU_PAD + MENU_CHECK_AREA, ty,
                                 it->labelW, it->labelH };
                SDL_RenderCopy(ren, it->labelTex, NULL, &dst);
            }
            if (it->shortTex) {
                SDL_SetTextureColorMod(it->shortTex, c, c, c);
                int ty = yacc + (itemH - it->shortH) / 2;
                SDL_Rect dst = { dr.x + dr.w - it->shortW - MENU_PAD, ty,
                                 it->shortW, it->shortH };
                SDL_RenderCopy(ren, it->shortTex, NULL, &dst);
            }
        }

        yacc += itemH;
    }
}

int MenuHandleEvent(SDL_Event *e)
{
    if (!gMenuReady) return 0;

    switch (e->type) {
    case SDL_KEYDOWN:
        if (e->key.keysym.sym == SDLK_ESCAPE && gMenuOpen >= 0) {
            gMenuOpen  = -1;
            gMenuHover = -1;
            return 1;
        }
        return 0;

    case SDL_MOUSEMOTION:
        if (gMenuOpen >= 0)
            gMenuHover = HitItem(gMenuOpen, e->motion.x, e->motion.y);
        return 0;

    case SDL_MOUSEBUTTONDOWN:
        if (e->button.button != SDL_BUTTON_LEFT) return 0;
        {
            int x = e->button.x, y = e->button.y;

            if (y < MENU_H) {
                int hit = HitTopLabel(x);
                if (hit < 0) {
                    gMenuOpen  = -1;
                    gMenuHover = -1;
                    return 0;
                }
                gMenuOpen  = (gMenuOpen == hit) ? -1 : hit;
                gMenuHover = -1;
                return 1;
            }

            if (gMenuOpen >= 0) {
                SDL_Rect dr = DropRect(gMenuOpen);
                if (x >= dr.x && x < dr.x + dr.w &&
                    y >= dr.y && y < dr.y + dr.h) {
                    int j = HitItem(gMenuOpen, x, y);
                    int m = gMenuOpen;
                    gMenuOpen  = -1;
                    gMenuHover = -1;
                    if (j >= 0) {
                        MenuItem *it = &gItems[m][j];
                        int grayed = (it->idm <= 0) || (it->needsVM && v.iVM < 0);
                        if (it->idm == IDM_D1U && v.iVM >= 0) grayed |= !rgpvm[v.iVM]->rgvd[0].sz[0];
                        if (it->idm == IDM_D2U && v.iVM >= 0) grayed |= !rgpvm[v.iVM]->rgvd[1].sz[0];
                        if (it->idm == IDM_WP1 && v.iVM >= 0)        grayed |= !rgpvm[v.iVM]->rgvd[0].sz[0];
                        if (it->idm == IDM_WP2 && v.iVM >= 0)        grayed |= !rgpvm[v.iVM]->rgvd[1].sz[0];
                        if (it->idm == IDM_IMPORTDOS1 && v.iVM >= 0) grayed |= !rgpvm[v.iVM]->rgvd[0].sz[0];
                        if (it->idm == IDM_IMPORTDOS2 && v.iVM >= 0) grayed |= !rgpvm[v.iVM]->rgvd[1].sz[0];
                        if (it->idm == IDM_NOCART && v.iVM >= 0)     grayed |= !rgpvm[v.iVM]->rgcart.fCartIn;
                        if (it->idm == IDM_DELVM)  grayed |= (v.cVM <= 1);
                        if (it->idm == IDM_NEXTVM) grayed |= (v.cVM <= 1);
                        if (it->idm == IDM_PREVVM) grayed |= (v.cVM <= 1);
                        if (!grayed)
                            DispatchMenuCmd(it->idm);
                    }
                    return 1;
                }
                gMenuOpen  = -1;
                gMenuHover = -1;
                return 0;
            }
        }
        return 0;

    default:
        return 0;
    }
}

#endif /* !_WIN32 */
