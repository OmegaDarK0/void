#include "void.h"

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
    void_memory_init_arena(1 * MB);
    void_engine_init();
    const VoidWindow *window = void_window_create("VOID", 1280, 720);
    SDL_Delay(1000);
    void_window_destroy(window);
    void_engine_quit();
    void_memory_quit();
    return 0;
}
