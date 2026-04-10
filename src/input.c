#include <SDL2/SDL.h>
#include "void.h"

bool void_input_is_key_pressed(const VoidKey key) {
    const uint8 *state = (uint8*)SDL_GetKeyboardState(NULL);
    switch (key) {
        case VOID_KEY_W: return state[SDL_SCANCODE_W];
        case VOID_KEY_A: return state[SDL_SCANCODE_A];
        case VOID_KEY_S: return state[SDL_SCANCODE_S];
        case VOID_KEY_D: return state[SDL_SCANCODE_D];
        case VOID_KEY_ESCAPE: return state[SDL_SCANCODE_ESCAPE];
        default: return false;
    }
}
