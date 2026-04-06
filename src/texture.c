#include "void.h"

VoidTexture *void_texture_load(const VoidWindow *window, const char *filename) {
    VoidTexture *texture = IMG_LoadTexture(void_window_get_render(window), filename);
    return texture;
}

void void_texture_destroy(VoidTexture *texture) {
    if (texture) SDL_DestroyTexture(texture);
}

uint8 void_texture_draw(const VoidWindow *window, VoidTexture *texture, const float x, const float y, const float w, const float h) {
    const SDL_FRect dst = {x, y, w, h};
    if (SDL_RenderCopyF(void_window_get_render(window), texture, NULL, &dst) < 0) {
        return VOID_FAILURE;
    }
    return VOID_SUCCESS;
}
