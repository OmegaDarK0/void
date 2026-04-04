#include "void.h"

#define FRAME_MEMORY_FACTOR 0.1
#define MEMORY_PRINT_FACTOR 25

typedef struct {
    uchar *base;
    ulong capacity;
    ulong offset;
} VoidArena;

static VoidArena s_global_arena, s_frame_arena;

void void_memory_print(void) {
    char buffer[MEMORY_PRINT_FACTOR + 1];
    const uchar global_alloc = MEMORY_PRINT_FACTOR * s_global_arena.offset / s_global_arena.capacity;
    const uchar global_free = MEMORY_PRINT_FACTOR - global_alloc;
    const uchar frame_alloc = MEMORY_PRINT_FACTOR * s_frame_arena.offset / s_frame_arena.capacity;
    const uchar frame_free = MEMORY_PRINT_FACTOR - frame_alloc;
    printf("--------------------------------------------------\n");
    for (uchar i = 0; i < global_alloc; i++) buffer[i] = '#';
    for (uchar i = 0; i < global_free; i++) buffer[global_alloc + i] = '.';
    buffer[MEMORY_PRINT_FACTOR] = '\0';
    printf("[GLOBAL] %s (%lu / %lu)\n", buffer, s_global_arena.offset, s_global_arena.capacity);
    for (uchar i = 0; i < frame_alloc; i++) buffer[i] = '#';
    for (uchar i = 0; i < frame_free; i++) buffer[frame_alloc + i] = '.';
    buffer[MEMORY_PRINT_FACTOR] = '\0';
    printf("[FRAME ] %s (%lu / %lu)\n", buffer, s_frame_arena.offset, s_frame_arena.capacity);
    printf("--------------------------------------------------\n");
}

uchar void_memory_init_arena(const ulong size) {
    s_global_arena.base = (uchar*)malloc(size);
    if (s_global_arena.base == NULL) {
        return VOID_ERROR;
    }
    s_frame_arena.capacity = (ulong)((double)size * FRAME_MEMORY_FACTOR);
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
    return VOID_OK;
}

void void_memory_quit(void) {
    free(s_global_arena.base);
    s_global_arena.base = NULL;
    s_frame_arena.base = NULL;
}

void* void_arena_alloc(const ulong size, const uint alignment) {
    const ulong address = (ulong)s_global_arena.base + s_global_arena.offset;
    const ulong aligned_address = (address + (alignment - 1)) & ~(ulong)(alignment - 1);
    const ulong padding = aligned_address - address;
    if (s_global_arena.offset + padding + size > s_global_arena.capacity) {
        return NULL;
    }
    s_global_arena.offset += padding + size;
    /*printf("Arena allocated :\n");
    printf("-current address : %lu\n", address);
    printf("-aligned address : %lu\n", aligned_address);
    printf("-address padding : %lu\n", padding);
    printf("-address offset : %lu\n", s_global_arena.offset);
    printf("----------------------------------------\n");*/
    return (void*)aligned_address;
}

void* void_frame_alloc(const ulong size, const uint alignment) {
    const ulong address = (ulong)s_frame_arena.base + s_frame_arena.offset;
    const ulong aligned_address = (address + (alignment - 1)) & ~((ulong)alignment - 1);
    const ulong padding = aligned_address - address;
    if (s_frame_arena.offset + padding + size > s_frame_arena.capacity) {
        return NULL;
    }
    s_frame_arena.offset += padding + size;
    /*printf("Frame allocated :\n");
    printf("-current address : %lu\n", address);
    printf("-aligned address : %lu\n", aligned_address);
    printf("-address padding : %lu\n", padding);
    printf("-address offset : %lu\n", s_frame_arena.offset);
    printf("----------------------------------------\n");*/
    return (void*)aligned_address;
}

void void_frame_free(void) {
    s_frame_arena.offset = 0;
}
