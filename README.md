# VOID (Platform Abstraction Layer)

*Read this in other languages: [Français](README.fr.md)*

**VOID** is a static library written in pure C (C11/C17) acting as a Hardware Abstraction Layer (HAL). It was designed to serve as the "Back-end" foundation for the **SOUL**, but can be used in any project requiring high-performance, low-level access.

Its core philosophy is **Strict Binary Isolation**: the library user never sees underlying dependencies and interacts only with a minimal and predictable interface contract (`void.h`).

---

## Architectural Principles

- **Zero Hidden Allocations**: Replaces erratic `malloc/free` calls with an Arena Allocator system for instantaneous allocation performance and maximum cache coherency.
- **Opaque by Design**: All hardware concepts (Windows, Rendering Contexts, Textures) are hidden behind opaque pointers (`VoidWindow`, `VoidTexture`).
- **Data-Oriented Ready**: Primitive types are strictly defined (`uint32`, `uint64`), guaranteeing data structure portability (SoA) across any CPU.

---

## SDK Content

The project compiles into a standalone static library. The complete API is exposed through a single header file:

- `include/void.h`: The public interface (The Contract).
- `lib/<os>-<arch>/libvoid.a`: The compiled binary to be linked statically.

---

## Compilation

VOID uses a robust `Makefile` supporting cross-compilation and generating both the library and a test suite.

### Prerequisites
- `gcc` or `mingw-w64` (for Windows)
- `ar` (Archiver)
- `libsdl2-dev`, `libsdl2-image-dev`

### Build Commands

```bash
#Compile the library (libvoid.a) and the test executable for the current OS:
make

#Compile in Debug mode:
make BUILD=debug

#Display toolchain information (useful for diagnostics):
make info

#Clean workspace:
make fclean
```

### Cross-Compilation (e.g., Windows from Linux)
```bash
make OS=windows ARCH=x86_64
```

---

## Using the Test Suite

A standalone test program is generated in the `bin/` folder. It allows for stress-testing different modules in isolation without needing the Front-end engine.

```bash
./bin/linux-x86_64/void [test_name]
```

Available arguments:
- `memory`: Tests persistent block allocation and frame arena rollback.
- `thread`: Tests atomic operations by saturating detected logical cores.
- `render`: Launches a visual render loop, validating delta time and display independence.
- `all`: Runs the full suite.

---

## Integrating into an External Project (C/C++)

To use **VOID** in a higher-level project, simply include the header and link the static library:

```makefile
# Makefile Integration Example
CFLAGS  += -I/path/to/void/include
LDFLAGS += -L/path/to/void/lib/linux-x86_64
LDLIBS  += -lvoid -lSDL2 -lSDL2_image -lm -pthread
```

*Note: While the `void.h` header hides SDL2, the final project's linker will still need SDL's system dependencies to resolve symbols in `libvoid.a`.*