#include "void.h"

uint8 s_init_state = 0;

uint8 void_engine_init(void) {
    if (s_init_state) {
        return VOID_FAILURE;
    }
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
        return VOID_FAILURE;
    }
    s_init_state |= VOID_INIT_SDL;
    return VOID_SUCCESS;
}

void void_engine_quit(void) {
    if (s_init_state & VOID_INIT_SDL) {
        s_init_state &= ~VOID_INIT_SDL;
        SDL_Quit();
    }
}
