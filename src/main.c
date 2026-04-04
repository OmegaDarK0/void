#include "void.h"

int main(void) {
    if (void_memory_init_arena(64 * MB)) {
        perror("void_memory_init_global_arena");
        exit(1);
    }
    void_arena_alloc(16 * MB, 2);
    void_frame_alloc(4 * MB, 2);
    void_memory_print();
    void_memory_quit();
    return 0;
}
