#include "void.h"

#define FRAME_MEMORY_FACTOR 0.1
#define MEMORY_PRINT_FACTOR 25

typedef struct {
    uint8 *base;
    uint64 capacity, offset, offset_prev;
} VoidArena;

static VoidArena s_global_arena, s_frame_arena;

void void_memory_print(void) {
    char buffer[MEMORY_PRINT_FACTOR + 1];
    buffer[MEMORY_PRINT_FACTOR] = '\0';
    const uint8 global_alloc = MEMORY_PRINT_FACTOR * s_global_arena.offset / s_global_arena.capacity;
    const uint8 global_free = MEMORY_PRINT_FACTOR - global_alloc;
    const uint8 frame_alloc = MEMORY_PRINT_FACTOR * s_frame_arena.offset / s_frame_arena.capacity;
    const uint8 frame_free = MEMORY_PRINT_FACTOR - frame_alloc;
    for (uint8 i = 0; i < global_alloc; i++) buffer[i] = '#';
    for (uint8 i = 0; i < global_free; i++) buffer[global_alloc + i] = '.';
    printf("[GLOBAL] %s (%lu / %lu)\n", buffer, s_global_arena.offset, s_global_arena.capacity);
    for (uint8 i = 0; i < frame_alloc; i++) buffer[i] = '#';
    for (uint8 i = 0; i < frame_free; i++) buffer[frame_alloc + i] = '.';
    printf("[FRAME ] %s (%lu / %lu)\n", buffer, s_frame_arena.offset, s_frame_arena.capacity);
    printf("--------------------------------------------------\n");
}

uint8 void_memory_init_arena(const uint64 size) {
    s_global_arena.base = (uint8*)malloc(size);
    if (s_global_arena.base == NULL) return VOID_FAILURE;
    s_frame_arena.capacity = (uint64)((double)size * FRAME_MEMORY_FACTOR);
    s_global_arena.capacity = size - s_frame_arena.capacity;
    s_frame_arena.base = s_global_arena.base + s_global_arena.capacity;
    s_global_arena.offset = 0;
    s_frame_arena.offset = 0;
    /*printf("Initialized global arena :\n");
    printf("-global arena base: %p\n", s_global_arena.base);
    printf("-global arena capacity: %lu\n", s_global_arena.capacity);
    printf("-global arena offset: %lu\n", s_global_arena.offset);
    printf("-frame arena base: %p\n", s_frame_arena.base);
    printf("-frame arena capacity: %lu\n", s_frame_arena.capacity);
    printf("-frame arena offset: %lu\n", s_frame_arena.offset);
    printf("----------------------------------------\n");*/
    return VOID_SUCCESS;
}

void void_memory_quit(void) {
    free(s_global_arena.base);
    s_global_arena.base = NULL;
    s_frame_arena.base = NULL;
}

void *void_arena_alloc(const uint64 size, const uint32 alignment) {
    const uint64 address = (uint64)s_global_arena.base + s_global_arena.offset;
    const uint64 aligned_address = (address + (alignment - 1)) & ~(uint64)(alignment - 1);
    const uint64 padding = aligned_address - address;
    if (s_global_arena.offset + padding + size > s_global_arena.capacity) return NULL;
    s_global_arena.offset_prev = s_global_arena.offset;
    s_global_arena.offset += padding + size;
    /*printf("Arena allocated :\n");
    printf("-current address : %lu\n", address);
    printf("-aligned address : %lu\n", aligned_address);
    printf("-address padding : %lu\n", padding);
    printf("-address offset : %lu\n", s_global_arena.offset);
    printf("----------------------------------------\n");*/
    return (void*)aligned_address;
}

void void_arena_rollback(void) {
    s_global_arena.offset = s_global_arena.offset_prev;
}

void *void_frame_alloc(const uint64 size, const uint32 alignment) {
    const uint64 address = (uint64)s_frame_arena.base + s_frame_arena.offset;
    const uint64 aligned_address = (address + (alignment - 1)) & ~((uint64)alignment - 1);
    const uint64 padding = aligned_address - address;
    if (s_frame_arena.offset + padding + size > s_frame_arena.capacity) return NULL;
    s_frame_arena.offset_prev = s_frame_arena.offset;
    s_frame_arena.offset += padding + size;
    /*printf("Frame allocated :\n");
    printf("-current address : %lu\n", address);
    printf("-aligned address : %lu\n", aligned_address);
    printf("-address padding : %lu\n", padding);
    printf("-address offset : %lu\n", s_frame_arena.offset);
    printf("----------------------------------------\n");*/
    return (void*)aligned_address;
}

void void_frame_rollback(void) {
    s_frame_arena.offset = s_frame_arena.offset_prev;
}

void void_frame_free(void) {
    s_frame_arena.offset = 0;
}
