#include "void.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define ANIMATION_SPEED 60
#define TOTAL_FRAMES 4
#define FRAME_WIDTH 32
#define FRAME_HEIGHT 32

int memory_test(void) {
    if (void_memory_init(64 * MB)) {
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
    void_memory_exit();
    return 0;
}

static volatile uint32 s_shared_counter = 0;

void thread_stress_task(void *data) {
    const uint32 increments_to_do = (uint32)(uintptr_t)data;
    for (uint32 i = 0; i < increments_to_do; i++) {
        void_atomic_increment(&s_shared_counter);
    }
}

void thread_test(void) {
    const uint32 cores = void_system_get_core_count();
    printf("[THREAD TEST] CPU detecte : %u coeurs logiques.\n", cores);
    const uint32 increments_per_thread = 1000000;
    const uint32 target = cores * increments_per_thread;
    s_shared_counter = 0;
    printf("[THREAD TEST] Lancement de %u threads...\n", cores);
    printf("[THREAD TEST] Objectif a atteindre : %u\n", target);
    for (uint32 i = 0; i < cores; i++) {
        void_thread_create(thread_stress_task, (void*)(uintptr_t)increments_per_thread);
    }
    while (s_shared_counter < target) {
        void_thread_sleep(10);
    }
    printf("[THREAD TEST] Test termine avec succes ! Compteur final : %u\n", s_shared_counter);
    printf("--------------------------------------------------\n");
}

int game_test(void) {
    void_init();
    VoidWindow *window = void_window_create("VOID", 1280, 720);
    VoidTexture *texture = void_texture_load(window, "assets/drone.png");
    const int entity_size = 256;
    float entity_pos_x = (float)WINDOW_WIDTH / 2 - (float)entity_size / 2;
    float entity_pos_y = (float)WINDOW_HEIGHT / 2 - (float)entity_size / 2;
    int r = 0, g = 0, b = 0;
    while (void_window_is_running(window)) {
        const uint64 ms = void_time_get_ticks();
        const uint64 s = ms / 1000;
        const float dt = void_time_get_delta();
        const int current_frame = (int)(ms / ANIMATION_SPEED) % TOTAL_FRAMES;
        void_window_poll_events(window);
        if (void_window_should_close(window)) {
            void_window_close(window);
        }
        if (void_input_is_key_pressed(SDL_SCANCODE_W)) entity_pos_y -= 500 * dt;
        if (void_input_is_key_pressed(SDL_SCANCODE_S)) entity_pos_y += 500 * dt;
        if (void_input_is_key_pressed(SDL_SCANCODE_A)) entity_pos_x -= 500 * dt;
        if (void_input_is_key_pressed(SDL_SCANCODE_D)) entity_pos_x += 500 * dt;
        void_render_clear(window, 0, 0, 0, 255);
        void_texture_draw(window, texture,
            current_frame * FRAME_WIDTH + current_frame, 0, FRAME_WIDTH, FRAME_HEIGHT,
            entity_pos_x, entity_pos_y, (float)entity_size, (float)entity_size);
        switch (s % 4) {
            case 0:
                r = 255;
                g = b = 0;
                break;
            case 1:
                b = 255;
                r = g = 0;
                break;
            case 2:
                g = 255;
                r = b = 0;
                break;
            case 3:
                r = g = 255;
                b = 0;
            default:
                break;
        }
        void_render_rect(window,
            entity_pos_x, entity_pos_y, (float)entity_size, (float)entity_size,
            r, g, b, 255, 0);
        void_render_present(window);
        void_log_flush();
        void_frame_free();
    }
    void_texture_destroy(texture);
    void_window_destroy(window);
    void_exit();
    return 0;
}

int main(const int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    return game_test();
}
