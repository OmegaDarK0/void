#ifndef VOID_H
#define VOID_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdalign.h>

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
    VOID_SUCCESS,
    VOID_FAILURE,
} VoidStatus;

typedef enum {
    VOID_INIT_SDL = 1 << 0,
    VOID_INIT_IMG = 1 << 1,
    VOID_INIT_MIX = 1 << 2,
} VoidFlags;

typedef unsigned char       uint8;
typedef unsigned short int  uint16;
typedef unsigned int        uint32;
typedef unsigned long int   uint64;

// ============================================================================
// SYSTÈME & TEMPS (core.c / time.c)
// ============================================================================

uint8 void_engine_init(void);
void void_engine_quit(void);

uint32 void_system_get_core_count(void); // Utile pour initialiser les workers (job.cpp)

uint64 void_time_get_ticks(void);        // Temps haute résolution
float void_time_get_delta(void);        // Delta time calculé par le Back-end

// ============================================================================
// MÉMOIRE (memory.c)
// ============================================================================
// Le Back-end alloue de gros blocs au démarrage via l'OS.
// Le Front-end (C++) demandera des sous-blocs à partir de ces arènes.

void void_memory_print(void);

uint8 void_memory_init_arena(uint64 size);
void void_memory_quit(void);

// Allocation depuis l'arène globale (persistante)
void *void_arena_alloc(uint64 size, uint32 alignment);
void void_arena_rollback(void);

// Arène temporaire (réinitialisée à chaque frame par le Front-end)
void *void_frame_alloc(uint64 size, uint32 alignment);
void void_frame_rollback(void);
void void_frame_free(void);

// ============================================================================
// FENÊTRE & ENTRÉES (window.c / input.c)
// ============================================================================
// Pointeur opaque : le Front-end n'a pas besoin de savoir ce qu'est une fenêtre SDL
typedef struct VoidWindow VoidWindow;
typedef SDL_Renderer VoidRender;
typedef SDL_Texture VoidTexture;

VoidWindow *void_window_create(const char *title, uint32 width, uint32 height);
void void_window_destroy(const VoidWindow *window);
void void_window_close(VoidWindow *window);
bool void_window_is_running(const VoidWindow *window);
bool void_window_should_close(const VoidWindow *window);
void void_window_poll_events(VoidWindow *window); // Récupère les messages de l'OS
VoidRender *void_window_get_render(const VoidWindow *window);

// Lecture de l'état du clavier sans callback/event listener
uint8 void_input_is_key_pressed(uint32 keycode);
uint8 void_input_is_mouse_button_pressed(uint8 button);

// ============================================================================
// RENDU 2D BASIQUE (Fourni par le back-end SDL_Renderer)
// ============================================================================
// Efface l'écran avec une couleur (R, G, B, A)
uint8 void_render_clear(const VoidWindow *window, uint8 r, uint8 g, uint8 b, uint8 a);

// Envoie l'image finale à l'écran (Swap Buffers)
void void_render_present(const VoidWindow *window);

uint8 void_render_point(const VoidWindow *window, float x, float y, uint8 r, uint8 g, uint8 b, uint8 a);
uint8 void_render_line(const VoidWindow *window, float x1, float y1, float x2, float y2, uint8 r, uint8 g, uint8 b, uint8 a);
// Dessine un rectangle plein
uint8 void_render_rect(const VoidWindow *window, float x, float y, float w, float h, uint8 r, uint8 g, uint8 b, uint8 a, bool fill);

VoidTexture *void_texture_load(const VoidWindow *window, const char *filename);
void void_texture_destroy(VoidTexture *texture);
uint8 void_texture_draw(const VoidWindow *window, VoidTexture *texture, float x, float y, float w, float h);

// ============================================================================
// THREADING BAS NIVEAU (thread.c)
// ============================================================================
// Fournit juste ce qu'il faut pour que job.cpp (C++) construise ses Fibers
typedef void (*VoidThreadFunc)(void *data);

void void_thread_create(VoidThreadFunc func, void *data);
void void_thread_sleep(uint32 milliseconds);

// Primitives atomiques pour éviter les mutex bloquants
uint32 void_atomic_increment(volatile uint32 *value);
uint32 void_atomic_decrement(volatile uint32 *value);
uint8 void_atomic_compare_exchange(volatile uint32 *value, uint32 expected, uint32 new_value);

#ifdef __cplusplus
}
#endif

#endif // VOID_H
