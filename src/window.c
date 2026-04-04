#include "void.h"

struct VoidWindow {
    SDL_Window* handle;
    SDL_Renderer* render;
    bool should_close;
};

static VoidWindow *s_window = NULL;

VoidWindow *void_window_create(const char *title, const uint32 width, const uint32 height) {
    VoidWindow* window = void_arena_alloc(sizeof(VoidWindow), alignof(VoidWindow));
    if (window == NULL) return NULL;
    window->handle = SDL_CreateWindow(title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        (int)width, (int)height,
        0
        );
    if (window->handle == NULL) {
        void_arena_rollback();
        return NULL;
    }
    window->render = SDL_CreateRenderer(window->handle, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
        );
    if (window->render == NULL) {
        SDL_DestroyWindow(window->handle);
        void_arena_rollback();
        return NULL;
    }
    window->should_close = false;
    s_window = window;
    return window;
}

void void_window_destroy(const VoidWindow *window) {
    if (window == NULL) return;
    SDL_DestroyRenderer(window->render);
    SDL_DestroyWindow(window->handle);
}

bool void_window_should_close(const VoidWindow *window) {
    return window->should_close;
}

