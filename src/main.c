#include "void.h"

#define WIDTH 1280
#define HEIGHT 720

int memory_test(void) {
    if (void_memory_init_arena(64 * MB)) {
        perror("void_memory_init_global_arena");
        exit(1);
    }
    void_arena_alloc(16 * MB, 2);
    void_frame_alloc(4 * MB, 2);
    void_memory_print();
    void_arena_alloc(8 * MB, 2);
    void_memory_print();
    void_frame_free();
    void_memory_print();
    void_arena_rollback();
    void_memory_print();
    void_memory_quit();
    return 0;
}

int main(void) {
    void_memory_init_arena(KB);
    void_engine_init();
    VoidWindow *window = void_window_create("VOID", 1280, 720);
    void_memory_print();
    const int size = 200;
    int posx = WIDTH / 2 - size / 2, posy = HEIGHT / 2 - size / 2;
    int r = 0, g = 0, b = 0;
    while (void_window_is_running(window)) {
        void_window_poll_events(window);
        if (void_window_should_close(window)) {
            void_window_close(window);
        }
        if (void_input_is_key_pressed(SDL_SCANCODE_W)) posy -= 5;
        if (void_input_is_key_pressed(SDL_SCANCODE_A)) posx -= 5;
        if (void_input_is_key_pressed(SDL_SCANCODE_S)) posy += 5;
        if (void_input_is_key_pressed(SDL_SCANCODE_D)) posx += 5;
        void_render_clear(window, 0, 0, 0, 255);
        const uint64 ms = void_time_get_ticks();
        const uint64 s = ms / 1000;
        switch (s % 3) {
            case 0:
                r = 255;
                g = b = 0;
                break;
            case 1:
                g = 255;
                r = b = 0;
                break;
            case 2:
                b = 255;
                r = g = 0;
                break;
            default:
                break;
        }
        void_render_rect(window,
            (float)posx, (float)posy, (float)size, (float)size,
            r, g, b, 255, 1);
        void_render_present(window);
        void_frame_free();
    }
    void_window_destroy(window);
    void_engine_quit();
    void_memory_quit();
    return 0;
}
