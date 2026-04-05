#include "void.h"

uint64 void_time_get_ticks(void) {
    return SDL_GetTicks64();
}
