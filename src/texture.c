#include "void.h"

VoidTexture *void_texture_load(const VoidWindow *window, const char *filename) {
    VoidTexture *texture = IMG_LoadTexture(void_window_get_render(window), filename);
    if (texture == NULL) VOID_LOG_ERROR("IMG_LoadTexture failed: %s", IMG_GetError());
    return texture;
}

void void_texture_destroy(VoidTexture *texture) {
    if (texture) SDL_DestroyTexture(texture);
}

uint8 void_texture_draw(const VoidWindow *window, VoidTexture *texture,
    const int src_x, const int src_y, const int src_w, const int src_h,
    const float dst_x, const float dst_y, const float dst_w, const float dst_h) {
    const SDL_Rect src = {src_x, src_y, src_w, src_h};
    const SDL_FRect dst = {dst_x, dst_y, dst_w, dst_h};
    if (SDL_RenderCopyF(void_window_get_render(window), texture, &src, &dst) < 0) {
        VOID_LOG_ERROR("SDL_RenderCopyF failed: %s", SDL_GetError());
        return VOID_FAILURE;
    }
    return VOID_SUCCESS;
}
