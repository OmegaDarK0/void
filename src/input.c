#include "void.h"

uint8 void_input_is_key_pressed(const uint32 keycode) {
    return SDL_GetKeyboardState(NULL)[keycode];
}
