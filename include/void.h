#ifndef VOID_H
#define VOID_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>
#include <string.h>

#define KB 1024
#define MB (1024 * KB)
#define GB (1024 * MB)

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// FUNDAMENTAL TYPES
// ============================================================================
// Guarantees data size, vital for Data-Oriented Design (SoA)

typedef enum {
    VOID_INIT_SDL = 1 << 0,
    VOID_INIT_IMG = 1 << 1,
    VOID_INIT_MIX = 1 << 2,
} VoidInitFlag;

typedef enum {
    VOID_LOG_OK,
    VOID_LOG_INFO,
    VOID_LOG_WARN,
    VOID_LOG_ERROR,
    VOID_LOG_FATAL
} VoidLogLevel;

typedef enum {
    VOID_KEY_W,
    VOID_KEY_A,
    VOID_KEY_S,
    VOID_KEY_D,
    VOID_KEY_UP,
    VOID_KEY_DOWN,
    VOID_KEY_LEFT,
    VOID_KEY_RIGHT,
    VOID_KEY_SPACE,
    VOID_KEY_ESCAPE
} VoidKeyCode;

typedef void (*VoidThreadFunc)(void *data);

typedef struct VoidThread   VoidThread;
typedef struct VoidLogEntry VoidLogEntry;

typedef unsigned char       uint8;
typedef unsigned short int  uint16;
typedef unsigned int        uint32;
typedef unsigned long int   uint64;

// ============================================================================
// SYSTEM & TIME (core.c / time.c)
// ============================================================================

bool   void_init(void);
void   void_exit(void);

uint32 void_system_get_core_count(void); // Useful for initializing workers (job.cpp)

uint64 void_time_get_ticks(void);        // High-resolution time in milliseconds
float  void_time_get_delta(void);        // Delta time calculated by the Back-end

// ============================================================================
// MEMORY (memory.c)
// ============================================================================
// The Back-end allocates large blocks at startup via the OS.
// The Front-end (C++) will request sub-blocks from these persistent arenas.

void  void_memory_print(void);

bool  void_memory_init(uint64 size);
void  void_memory_exit(void);

// Allocation from the global arena (persistent throughout the engine lifecycle)
void *void_arena_alloc(uint64 size, uint32 alignment);
void  void_arena_rollback(void);

// Temporary arena (reset every frame by the Front-end, zero-cost allocation)
void *void_frame_alloc(uint64 size, uint32 alignment);
void  void_frame_rollback(void);
void  void_frame_free(void);

// ============================================================================
// WINDOW & INPUT (window.c / input.c)
// ============================================================================
// Opaque pointers: the Front-end doesn't need to know what an SDL window is

typedef struct VoidWindow  VoidWindow;
typedef struct VoidRender  VoidRender;
typedef struct VoidTexture VoidTexture;

VoidWindow *void_window_create(const char *title, uint32 width, uint32 height);
void        void_window_destroy(const VoidWindow *window);
void        void_window_close(VoidWindow *window);
bool        void_window_is_running(const VoidWindow *window);
bool        void_window_should_close(const VoidWindow *window);
void        void_window_poll_events(VoidWindow *window); // Polls OS messages
VoidRender *void_window_get_render(const VoidWindow *window);
void        void_window_get_size(const VoidWindow *window, int *width, int *height);

// Reads keyboard state instantly without callbacks/event listeners
bool        void_input_is_key_pressed(VoidKeyCode key);
//bool      void_input_is_mouse_button_pressed(uint8 button);

// ============================================================================
// BASIC 2D RENDERING (Provided by the SDL_Renderer back-end)
// ============================================================================

typedef struct {
    float x, y;
} VoidPoint;

typedef struct {
    float x, y, w, h;
} VoidRect;

// Clears the screen with a color (R, G, B, A)
bool void_render_clear(const VoidWindow *window, uint8 r, uint8 g, uint8 b, uint8 a);

// Presents the final image to the screen (Swap Buffers)
void void_render_present(const VoidWindow *window);

// Drawing primitives
bool void_render_point(const VoidWindow *window, float x, float y, uint8 r, uint8 g, uint8 b, uint8 a);
bool void_render_line(const VoidWindow *window, float x1, float y1, float x2, float y2, uint8 r, uint8 g, uint8 b, uint8 a);
bool void_render_rect(const VoidWindow *window, float x, float y, float w, float h, uint8 r, uint8 g, uint8 b, uint8 a, bool fill);

bool void_render_points(const VoidWindow *window, const VoidPoint *points, int count, uint8 r, uint8 g, uint8 b, uint8 a);
bool void_render_lines(const VoidWindow *window, const VoidPoint *points, int count, uint8 r, uint8 g, uint8 b, uint8 a);
bool void_render_rects(const VoidWindow *window, const VoidRect *rects, int count, uint8 r, uint8 g, uint8 b, uint8 a, bool fill);

// Texture management
VoidTexture *void_texture_load(const VoidWindow *window, const char *filename);
void         void_texture_destroy(const VoidTexture *texture);
bool         void_texture_draw(const VoidWindow *window, const VoidTexture *texture, int src_x, int src_y, int src_w, int src_h, float dst_x, float dst_y, float dst_w, float dst_h);

// Texture getters (Cached internally)
int void_texture_get_width(const VoidTexture *texture);
int void_texture_get_height(const VoidTexture *texture);

// ============================================================================
// LOW-LEVEL THREADING (thread.c)
// ============================================================================
// Provides just enough primitives for job.cpp (C++) to build its Fibers

void   void_thread_create(VoidThreadFunc func, void *data);
void   void_thread_sleep(uint32 ms);
uint32 void_thread_get_id(void);

// Atomic primitives to avoid blocking mutexes
uint32 void_atomic_increment(volatile uint32 *value);
uint32 void_atomic_decrement(volatile uint32 *value);
bool   void_atomic_compare_exchange(volatile uint32 *value, uint32 expected, uint32 desired);

// ============================================================================
// LOGGING SYSTEM (log.c)
// ============================================================================

void void_log_init(void);
void void_log_push(VoidLogLevel level, const char *file, int line, const char *fmt, ...);
void void_log_flush(void);
void void_log_exit(void);

// Helper macros to automatically inject file and line numbers
#define VOID_LOG_INFO(...)  void_log_push(VOID_LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define VOID_LOG_OK(...)    void_log_push(VOID_LOG_OK, __FILE__, __LINE__, __VA_ARGS__)
#define VOID_LOG_WARN(...)  void_log_push(VOID_LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define VOID_LOG_ERROR(...) void_log_push(VOID_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define VOID_LOG_FATAL(...) void_log_push(VOID_LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // VOID_H