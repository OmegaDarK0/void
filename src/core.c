#include "void.h"

#define SDL_INIT_FLAGS (SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO)
#define IMG_INIT_FLAGS (IMG_INIT_JPG | IMG_INIT_PNG)

uint8 s_init_state = 0;

uint8 void_engine_init(void) {
    if (s_init_state) {
        return VOID_FAILURE;
    }
    if (SDL_Init(SDL_INIT_FLAGS) < 0) {
        return VOID_FAILURE;
    }
    s_init_state |= VOID_INIT_SDL;
    if ((IMG_Init(IMG_INIT_FLAGS) & IMG_INIT_FLAGS) != IMG_INIT_FLAGS) {
        return VOID_FAILURE;
    }
    s_init_state |= VOID_INIT_IMG;
    return VOID_SUCCESS;
}

void void_engine_quit(void) {
    if (s_init_state & VOID_INIT_IMG) {
        s_init_state &= ~VOID_INIT_IMG;
        IMG_Quit();
    }
    if (s_init_state & VOID_INIT_SDL) {
        s_init_state &= ~VOID_INIT_SDL;
        SDL_Quit();
    }
}

uint32 void_system_get_core_count(void) {
    return SDL_GetCPUCount();
}
