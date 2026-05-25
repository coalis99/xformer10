#ifndef _WIN32
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <libgen.h>
#include "sdl_filebrowser.h"

#define FONT_PATH    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
#define FB_FONT_SIZE 14
#define FB_OVL_W     600
#define FB_OVL_H     440
#define FB_PATH_H     28
#define FB_ITEM_H     22
#define FB_PAD         8
#define FB_MAX_ENTRIES 512

typedef struct {
    char name[256];
    int  is_dir;
} FbEntry;

static int fb_ext_ok(const char *name)
{
    const char *dot = strrchr(name, '.');
    if (!dot) return 0;
    return strcasecmp(dot, ".atr") == 0 ||
           strcasecmp(dot, ".atx") == 0 ||
           strcasecmp(dot, ".xfd") == 0;
}

static int fb_cmp(const void *a, const void *b)
{
    const FbEntry *ea = (const FbEntry *)a;
    const FbEntry *eb = (const FbEntry *)b;
    if (ea->is_dir != eb->is_dir)
        return eb->is_dir - ea->is_dir;  /* dirs first */
    return strcasecmp(ea->name, eb->name);
}

static int fb_load_dir(const char *path, FbEntry *entries, int max)
{
    DIR *dp = opendir(path);
    if (!dp) return 0;
    int n = 0;
    struct dirent *de;
    while ((de = readdir(dp)) != NULL && n < max) {
        if (strcmp(de->d_name, ".") == 0) continue;
        char full[PATH_MAX];
        snprintf(full, sizeof(full), "%s/%s", path, de->d_name);
        struct stat st;
        if (stat(full, &st) != 0) continue;
        int is_dir = S_ISDIR(st.st_mode);
        if (!is_dir && !fb_ext_ok(de->d_name)) continue;
        strncpy(entries[n].name, de->d_name, 255);
        entries[n].name[255] = '\0';
        entries[n].is_dir    = is_dir;
        n++;
    }
    closedir(dp);
    qsort(entries, n, sizeof(FbEntry), fb_cmp);
    return n;
}

static void fb_render(SDL_Renderer *ren, SDL_Window *win, TTF_Font *font,
                      const char *cwd, FbEntry *entries, int nEntries,
                      int selected, int scroll)
{
    int winW, winH;
    SDL_GetWindowSize(win, &winW, &winH);

    int ox = (winW - FB_OVL_W) / 2;
    int oy = (winH - FB_OVL_H) / 2;

    /* overlay background */
    SDL_SetRenderDrawColor(ren, 35, 35, 35, 255);
    SDL_Rect ovl = {ox, oy, FB_OVL_W, FB_OVL_H};
    SDL_RenderFillRect(ren, &ovl);

    /* outline */
    SDL_SetRenderDrawColor(ren, 110, 110, 110, 255);
    SDL_RenderDrawRect(ren, &ovl);

    /* path bar background */
    SDL_SetRenderDrawColor(ren, 25, 35, 60, 255);
    SDL_Rect pathBar = {ox, oy, FB_OVL_W, FB_PATH_H};
    SDL_RenderFillRect(ren, &pathBar);

    /* path text */
    SDL_Color white = {230, 230, 230, 255};
    SDL_Surface *ps = TTF_RenderUTF8_Blended(font, cwd, white);
    if (ps) {
        SDL_Texture *pt = SDL_CreateTextureFromSurface(ren, ps);
        int tw = ps->w, th = ps->h;
        SDL_FreeSurface(ps);
        /* right-align / clip to bar width */
        int tx = ox + FB_OVL_W - tw - FB_PAD;
        if (tx < ox + FB_PAD) tx = ox + FB_PAD;
        SDL_Rect dst = {tx, oy + (FB_PATH_H - th) / 2, tw, th};
        SDL_RenderCopy(ren, pt, NULL, &dst);
        SDL_DestroyTexture(pt);
    }

    /* separator */
    SDL_SetRenderDrawColor(ren, 80, 80, 80, 255);
    SDL_RenderDrawLine(ren, ox, oy + FB_PATH_H, ox + FB_OVL_W - 1, oy + FB_PATH_H);

    /* set clip to list area */
    SDL_Rect listClip = {ox + 1, oy + FB_PATH_H + 1, FB_OVL_W - 2, FB_OVL_H - FB_PATH_H - 2};
    SDL_RenderSetClipRect(ren, &listClip);

    int visRows = (FB_OVL_H - FB_PATH_H) / FB_ITEM_H;
    for (int i = scroll; i < nEntries && i < scroll + visRows; i++) {
        int rowY = oy + FB_PATH_H + (i - scroll) * FB_ITEM_H;

        if (i == selected) {
            SDL_SetRenderDrawColor(ren, 60, 100, 170, 255);
            SDL_Rect hr = {ox + 1, rowY, FB_OVL_W - 2, FB_ITEM_H};
            SDL_RenderFillRect(ren, &hr);
        }

        SDL_Color fc = entries[i].is_dir
                       ? (SDL_Color){140, 190, 255, 255}
                       : (SDL_Color){220, 220, 220, 255};

        char label[258];
        if (entries[i].is_dir)
            snprintf(label, sizeof(label), "%s/", entries[i].name);
        else
            snprintf(label, sizeof(label), "%s",  entries[i].name);

        SDL_Surface *ls = TTF_RenderUTF8_Blended(font, label, fc);
        if (ls) {
            SDL_Texture *lt = SDL_CreateTextureFromSurface(ren, ls);
            int th = ls->h;
            SDL_FreeSurface(ls);
            SDL_Rect dst = {ox + FB_PAD, rowY + (FB_ITEM_H - th) / 2, 0, th};
            /* re-query width from texture */
            int tw2; SDL_QueryTexture(lt, NULL, NULL, &tw2, NULL);
            dst.w = tw2;
            SDL_RenderCopy(ren, lt, NULL, &dst);
            SDL_DestroyTexture(lt);
        }
    }

    SDL_RenderSetClipRect(ren, NULL);
}

int SDL_FileBrowserRun(SDL_Renderer *ren, SDL_Window *win,
                       const char *start_path, char *out, int sz)
{
    (void)out; (void)sz;

    TTF_Font *font = TTF_OpenFont(FONT_PATH, FB_FONT_SIZE);
    if (!font) return 0;

    char cwd[PATH_MAX];
    if (start_path && start_path[0] != '\0') {
        char tmp[PATH_MAX];
        strncpy(tmp, start_path, PATH_MAX - 1);
        tmp[PATH_MAX - 1] = '\0';
        char *dir = dirname(tmp);
        struct stat st;
        if (stat(dir, &st) == 0 && S_ISDIR(st.st_mode)) {
            strncpy(cwd, dir, PATH_MAX - 1);
            cwd[PATH_MAX - 1] = '\0';
        } else {
            const char *home = getenv("HOME");
            strncpy(cwd, home ? home : "/", PATH_MAX - 1);
            cwd[PATH_MAX - 1] = '\0';
        }
    } else {
        const char *home = getenv("HOME");
        strncpy(cwd, home ? home : "/", PATH_MAX - 1);
        cwd[PATH_MAX - 1] = '\0';
    }

    static FbEntry entries[FB_MAX_ENTRIES];
    int nEntries = fb_load_dir(cwd, entries, FB_MAX_ENTRIES);
    int selected = 0, scroll = 0;

    fb_render(ren, win, font, cwd, entries, nEntries, selected, scroll);
    SDL_RenderPresent(ren);

    TTF_CloseFont(font);
    return 0;
}
#endif /* !_WIN32 */
