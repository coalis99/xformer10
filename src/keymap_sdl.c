#ifndef _WIN32

#include <SDL2/SDL.h>
#include "gemtypes.h"

/* Forward: SDL scancode -> Win32 VK code (0 = unmapped) */
const int sdl_to_vk[512] = {
    /* Letters A-Z: SDL 4-29, VK 0x41-0x5A */
    [4]  = 'A', [5]  = 'B', [6]  = 'C', [7]  = 'D', [8]  = 'E',
    [9]  = 'F', [10] = 'G', [11] = 'H', [12] = 'I', [13] = 'J',
    [14] = 'K', [15] = 'L', [16] = 'M', [17] = 'N', [18] = 'O',
    [19] = 'P', [20] = 'Q', [21] = 'R', [22] = 'S', [23] = 'T',
    [24] = 'U', [25] = 'V', [26] = 'W', [27] = 'X', [28] = 'Y',
    [29] = 'Z',
    /* Digits 1-9, 0: SDL 30-39, VK 0x31-0x39, 0x30 */
    [30] = '1', [31] = '2', [32] = '3', [33] = '4', [34] = '5',
    [35] = '6', [36] = '7', [37] = '8', [38] = '9', [39] = '0',
    /* Control keys */
    [40] = VK_RETURN,   /* SDL_SCANCODE_RETURN */
    [41] = VK_ESCAPE,   /* SDL_SCANCODE_ESCAPE */
    [42] = VK_BACK,     /* SDL_SCANCODE_BACKSPACE */
    [43] = VK_TAB,      /* SDL_SCANCODE_TAB */
    [44] = VK_SPACE,    /* SDL_SCANCODE_SPACE */
    /* Punctuation (Win32 OEM VK codes) */
    [45] = 0xBD,  /* SDL_SCANCODE_MINUS      -> VK_OEM_MINUS */
    [46] = 0xBB,  /* SDL_SCANCODE_EQUALS     -> VK_OEM_PLUS */
    [47] = 0xDB,  /* SDL_SCANCODE_LEFTBRACKET  -> VK_OEM_4 */
    [48] = 0xDD,  /* SDL_SCANCODE_RIGHTBRACKET -> VK_OEM_6 */
    [49] = 0xDC,  /* SDL_SCANCODE_BACKSLASH   -> VK_OEM_5 */
    [51] = 0xBA,  /* SDL_SCANCODE_SEMICOLON   -> VK_OEM_1 */
    [52] = 0xDE,  /* SDL_SCANCODE_APOSTROPHE  -> VK_OEM_7 */
    [53] = 0xC0,  /* SDL_SCANCODE_GRAVE       -> VK_OEM_3 */
    [54] = 0xBC,  /* SDL_SCANCODE_COMMA       -> VK_OEM_COMMA */
    [55] = 0xBE,  /* SDL_SCANCODE_PERIOD      -> VK_OEM_PERIOD */
    [56] = 0xBF,  /* SDL_SCANCODE_SLASH       -> VK_OEM_2 */
    [57] = VK_CAPITAL,  /* SDL_SCANCODE_CAPSLOCK */
    /* Function keys F1-F12: SDL 58-69 */
    [58] = VK_F1,  [59] = VK_F2,  [60] = VK_F3,  [61] = VK_F4,
    [62] = VK_F5,  [63] = VK_F6,  [64] = VK_F7,  [65] = VK_F8,
    [66] = VK_F9,  [67] = VK_F10, [68] = VK_F11, [69] = VK_F12,
    /* Navigation cluster */
    [71] = VK_SCROLL,  /* SDL_SCANCODE_SCROLLLOCK */
    [72] = VK_PAUSE,   /* SDL_SCANCODE_PAUSE */
    [73] = VK_INSERT,  /* SDL_SCANCODE_INSERT */
    [74] = VK_HOME,    /* SDL_SCANCODE_HOME */
    [75] = VK_PRIOR,   /* SDL_SCANCODE_PAGEUP */
    [76] = VK_DELETE,  /* SDL_SCANCODE_DELETE */
    [77] = VK_END,     /* SDL_SCANCODE_END */
    [78] = VK_NEXT,    /* SDL_SCANCODE_PAGEDOWN */
    [79] = VK_RIGHT,   /* SDL_SCANCODE_RIGHT */
    [80] = VK_LEFT,    /* SDL_SCANCODE_LEFT */
    [81] = VK_DOWN,    /* SDL_SCANCODE_DOWN */
    [82] = VK_UP,      /* SDL_SCANCODE_UP */
    [83] = VK_NUMLOCK, /* SDL_SCANCODE_NUMLOCKCLEAR */
    /* Modifier keys */
    [224] = VK_LCONTROL, /* SDL_SCANCODE_LCTRL */
    [225] = VK_LSHIFT,   /* SDL_SCANCODE_LSHIFT */
    [226] = VK_LMENU,    /* SDL_SCANCODE_LALT */
    [228] = VK_RCONTROL, /* SDL_SCANCODE_RCTRL */
    [229] = VK_RSHIFT,   /* SDL_SCANCODE_RSHIFT */
    [230] = VK_RMENU,    /* SDL_SCANCODE_RALT */
};

/* Reverse: Win32 VK code -> SDL scancode (0 = unmapped) */
static int vk_to_sdl[256];

static void build_reverse(void)
{
    for (int sc = 1; sc < 512; sc++) {
        int vk = sdl_to_vk[sc];
        if (vk > 0 && vk < 256 && vk_to_sdl[vk] == 0)
            vk_to_sdl[vk] = sc;
    }
    /* Map generic shift/ctrl/alt to left-side variants */
    if (!vk_to_sdl[VK_SHIFT])   vk_to_sdl[VK_SHIFT]   = 225;
    if (!vk_to_sdl[VK_CONTROL]) vk_to_sdl[VK_CONTROL] = 224;
    if (!vk_to_sdl[VK_MENU])    vk_to_sdl[VK_MENU]    = 226;
}

SHORT sdl_get_async_key_state(int vk)
{
    static int initialized = 0;
    if (!initialized) { build_reverse(); initialized = 1; }
    if (vk < 0 || vk >= 256) return 0;
    int sc = vk_to_sdl[vk];
    if (sc == 0) return 0;
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    return state[sc] ? (SHORT)0x8000 : 0;
}

#endif /* !_WIN32 */
