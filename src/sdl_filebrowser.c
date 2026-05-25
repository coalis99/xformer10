#ifndef _WIN32
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "sdl_filebrowser.h"

#define FONT_PATH    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
#define FB_FONT_SIZE 14
#define FB_OVL_W     600
#define FB_OVL_H     440
#define FB_PATH_H     28
#define FB_ITEM_H     22
#define FB_PAD         8
#define FB_MAX_ENTRIES 512

int SDL_FileBrowserRun(SDL_Renderer *ren, SDL_Window *win,
                       const char *start_path, char *out, int sz)
{
    (void)ren; (void)win; (void)start_path; (void)out; (void)sz;
    return 0;
}
#endif /* !_WIN32 */
