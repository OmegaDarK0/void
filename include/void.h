#ifndef VOID_H
#define VOID_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define KB 1024
#define MB (1024 * KB)
#define GB (1024 * MB)

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// TYPES FONDAMENTAUX
// ============================================================================
// Garantie de la taille des données, vital pour le Data-Oriented Design (SoA)
typedef enum {
    VOID_OK,
    VOID_ERROR,
    VOID_WARNING,
    VOID_INFO
} VoidStatus;

typedef unsigned char       uchar;
typedef unsigned short int  ushort;
typedef unsigned int        uint;
typedef unsigned long int   ulong;

typedef unsigned char       uint8;
typedef unsigned short int  uint16;
typedef unsigned int        uint32;
typedef unsigned long int   uint64;

typedef signed char         int8;
typedef signed short int    int16;
typedef signed int          int32;
typedef signed long int     int64;

typedef float               float32;
typedef double              float64;
typedef long double         float80;

// ============================================================================
// SYSTÈME & TEMPS (system.c / time.c)
// ============================================================================
void void_system_init(void);
void void_system_shutdown(void);

uint32 void_system_get_core_count(void); // Utile pour initialiser les workers (job.cpp)

uint64 void_time_get_ticks(void);        // Temps haute résolution
float32 void_time_get_delta(void);        // Delta time calculé par le Back-end

// ============================================================================
// MÉMOIRE (memory.c)
// ============================================================================
// Le Back-end alloue de gros blocs au démarrage via l'OS.
// Le Front-end (C++) demandera des sous-blocs à partir de ces arènes.

void void_memory_print(void);

uint8 void_memory_init_arena(uint64 size);
void void_memory_quit(void);

// Allocation depuis l'arène globale (persistante)
void* void_arena_alloc(uint64 size, uint32 alignment);

// Arène temporaire (réinitialisée à chaque frame par le Front-end)
void* void_frame_alloc(uint64 size, uint32 alignment);
void void_frame_free(void);

// ============================================================================
// FENÊTRE & ENTRÉES (window.c / input.c)
// ============================================================================
// Pointeur opaque : le Front-end n'a pas besoin de savoir ce qu'est une fenêtre SDL
typedef SDL_Window VoidWindow;

VoidWindow* void_window_create(const char* title, uint32 width, uint32 height);
void void_window_destroy(VoidWindow* window);
uint8 void_window_should_close(VoidWindow* window);
void void_window_poll_events(void); // Récupère les messages de l'OS

// Lecture de l'état du clavier sans callback/event listener
uint8 void_input_is_key_pressed(uint32 keycode);
uint8 void_input_is_mouse_button_pressed(uint8 button);

// ============================================================================
// THREADING BAS NIVEAU (thread.c)
// ============================================================================
// Fournit juste ce qu'il faut pour que job.cpp (C++) construise ses Fibers
typedef void (*VoidThreadFunc)(void* data);

void void_thread_create(VoidThreadFunc func, void* data);
void void_thread_sleep(uint32 milliseconds);

// Primitives atomiques pour éviter les mutex bloquants
uint32 void_atomic_increment(volatile uint32* value);
uint32 void_atomic_decrement(volatile uint32* value);
uint8 void_atomic_compare_exchange(volatile uint32* value, uint32 expected, uint32 new_value);

#ifdef __cplusplus
}
#endif

#endif // VOID_H
