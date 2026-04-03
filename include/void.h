#ifndef VOID_H
#define VOID_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// TYPES FONDAMENTAUX
// ============================================================================
// Garantie de la taille des données, vital pour le Data-Oriented Design (SoA)
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t  i32;
typedef float    f32;
typedef double   f64;

// ============================================================================
// SYSTÈME & TEMPS (system.c / time.c)
// ============================================================================
void void_system_init(void);
void void_system_shutdown(void);

u32  void_system_get_core_count(void); // Utile pour initialiser les workers (job.cpp)

u64  void_time_get_ticks(void);        // Temps haute résolution
f32  void_time_get_delta(void);        // Delta time calculé par le Back-end

// ============================================================================
// MÉMOIRE (memory.c)
// ============================================================================
// Le Back-end alloue de gros blocs au démarrage via l'OS.
// L'Éther (C++) demandera des sous-blocs à partir de ces arènes.

bool void_memory_init_global_arena(u64 size_in_bytes);
void void_memory_shutdown(void);

// Allocation depuis l'arène globale (persistante)
void* void_arena_alloc(u64 size, u32 alignment);

// Arène temporaire (réinitialisée à chaque frame par l'Éther)
void* void_frame_alloc(u64 size, u32 alignment);
void  void_frame_clear(void);

// ============================================================================
// FENÊTRE & ENTRÉES (window.c / input.c)
// ============================================================================
// Pointeur opaque : l'Éther n'a pas besoin de savoir ce qu'est une fenêtre SDL
typedef struct VoidWindow VoidWindow; 

VoidWindow* void_window_create(const char* title, u32 width, u32 height);
void        void_window_destroy(VoidWindow* window);
bool        void_window_should_close(VoidWindow* window);
void        void_window_poll_events(void); // Récupère les messages de l'OS

// Lecture de l'état du clavier sans callback/event listener
bool void_input_is_key_pressed(u32 keycode);
bool void_input_is_mouse_button_pressed(u8 button);

// ============================================================================
// THREADING BAS NIVEAU (thread.c)
// ============================================================================
// Fournit juste ce qu'il faut pour que job.cpp (C++) construise ses Fibers
typedef void (*VoidThreadFunc)(void* data);

void void_thread_create(VoidThreadFunc func, void* data);
void void_thread_sleep(u32 milliseconds);

// Primitives atomiques pour éviter les mutex bloquants
u32  void_atomic_increment(volatile u32* value);
u32  void_atomic_decrement(volatile u32* value);
bool void_atomic_compare_exchange(volatile u32* value, u32 expected, u32 new_value);

#ifdef __cplusplus
}
#endif

#endif // VOID_H
