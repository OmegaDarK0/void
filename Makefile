VERSION   := 0.1.0-alpha

ifeq ($(OS),Windows_NT)
    HOST_OS   := windows
    HOST_ARCH := $(PROCESSOR_ARCHITECTURE)

    BLUE   :=
    GREEN  :=
    YELLOW :=
    NC     :=
else
    RAW_OS   := $(shell uname -s 2>/dev/null || echo Unknown)
    RAW_ARCH := $(shell uname -m 2>/dev/null || echo Unknown)

    ifeq ($(RAW_OS),Darwin)
        #HOST_OS := macOS
        $(error "Darwin is not yet implemented.")
    else ifeq ($(RAW_OS),Linux)
        HOST_OS := linux
    else
        $(error "Unsupported OS: $(RAW_OS)")
    endif

    ifeq ($(RAW_ARCH),amd64)
        HOST_ARCH := x86_64
    else ifeq ($(RAW_ARCH),x86_64)
        HOST_ARCH := x86_64
    else ifeq ($(RAW_ARCH),aarch64)
        #HOST_ARCH := arm64
        $(error "arm64 is not yet implemented.")
    else ifeq ($(RAW_ARCH),arm64)
        #HOST_ARCH := arm64
        $(error "arm64 is not yet implemented.")
    else ifneq (,$(filter i%86,$(RAW_ARCH)))
        HOST_ARCH := i386
    else
        $(error "Unsupported Architecture: $(RAW_ARCH)")
    endif

    BLUE   := \033[0;34m
    GREEN  := \033[0;32m
    YELLOW := \033[0;33m
    NC     := \033[0m
endif

OS   ?= $(HOST_OS)
ARCH ?= x86_64

TARGET_OS   := $(OS)
TARGET_ARCH := $(ARCH)

CC       := gcc
CXX      := g++
EXEC_EXT :=

ifeq ($(TARGET_OS),windows)
    EXEC_EXT := .exe
    LDLIBS   := -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows
    OS_CFLAGS  := -Dmain=SDL_main
    OS_LDFLAGS :=

    ifneq ($(HOST_OS),windows)
        ifeq ($(TARGET_ARCH),x86_64)
            CC  := x86_64-w64-mingw32-gcc
            CXX := x86_64-w64-mingw32-g++
        else
            CC  := i686-w64-mingw32-gcc
            CXX := i686-w64-mingw32-g++
        endif
    endif
else ifeq ($(TARGET_OS),linux)
    EXEC_EXT :=
    LDLIBS   := -lSDL2 -lSDL2_image -lm -ldl -pthread
    OS_CFLAGS  :=
    OS_LDFLAGS := -Wl,--enable-new-dtags -Wl,-rpath='$$ORIGIN/../../lib/$(TARGET_OS)_$(TARGET_ARCH)'
endif

ifeq ($(TARGET_ARCH),i386)
    ARCH_FLAGS := -m32
else
    ARCH_FLAGS := -m64
endif

EXEC      := void$(EXEC_EXT)

SRC_DIR   := src
INC_DIR   := include

LIB_DIR   := lib/$(TARGET_OS)-$(TARGET_ARCH)
BIN_DIR   := bin/$(TARGET_OS)-$(TARGET_ARCH)
BUILD_DIR := build/$(TARGET_OS)-$(TARGET_ARCH)

OBJ_DIR   := $(BUILD_DIR)/obj
DEP_DIR   := $(BUILD_DIR)/dep

CFLAGS    := $(ARCH_FLAGS) -std=c17 -Wall -Wextra -I$(INC_DIR) -DVERSION=\"$(VERSION)\" $(OS_CFLAGS)
CXXFLAGS  := $(ARCH_FLAGS) -std=c++20 -Wall -Wextra -I$(INC_DIR) -DVERSION=\"$(VERSION)\" $(OS_CFLAGS)

LDFLAGS   := $(ARCH_FLAGS) -static-libgcc -static-libstdc++ -L$(LIB_DIR) $(OS_LDFLAGS)
DEPFLAGS   = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

BUILD ?= release
ifeq ($(BUILD),debug)
    CFLAGS   += -Og -g -D_DEBUG
    CXXFLAGS += -Og -g -D_DEBUG
else
    CFLAGS   += -O2 -fstack-protector-strong -DNDEBUG
    CXXFLAGS += -O2 -fstack-protector-strong -DNDEBUG
    LDFLAGS  += -s
    ifeq ($(TARGET_OS),linux)
        LDFLAGS  += -Wl,-z,relro,-z,now
    endif
endif

SRCS_C    := $(shell find $(SRC_DIR) -type f -name "*.c")
SRCS_CPP  := $(shell find $(SRC_DIR) -type f -name "*.cpp")

OBJS      := $(SRCS_C:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o) \
             $(SRCS_CPP:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

DEPS      := $(SRCS_C:$(SRC_DIR)/%.c=$(DEP_DIR)/%.d) \
             $(SRCS_CPP:$(SRC_DIR)/%.cpp=$(DEP_DIR)/%.d)

TARGET_BIN := $(BIN_DIR)/$(EXEC)

.PHONY: all clean fclean re run debug

all: $(TARGET_BIN)

$(TARGET_BIN): $(OBJS)
	@mkdir -p $(dir $@)
	@echo "$(BLUE)[LD] Linking: $@$(NC)"
	@$(CXX) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@
ifeq ($(TARGET_OS),windows)
	@echo "$(YELLOW)[DLL] Copying dynamic libraries...$(NC)"
	@cp $(LIB_DIR)/*.dll $(BIN_DIR)/ 2>/dev/null || true
endif

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEP_DIR)/$*.d)
	@echo "$(YELLOW)[CC] $<$(NC)"
	@$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEP_DIR)/$*.d)
	@echo "$(YELLOW)[CXX] $<$(NC)"
	@$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

-include $(DEPS)

run: all
	@echo "$(GREEN)[RUN] Launching...$(NC)"
	@$(TARGET_BIN)

debug:
	@$(MAKE) --no-print-directory all BUILD=debug
	@gdb $(TARGET_BIN)

clean:
	@rm -rf $(BUILD_DIR)
	@echo "$(BLUE)[CLEAN] Build directory removed.$(NC)"

fclean: clean
	@rm -rf $(BIN_DIR)
	@echo "$(BLUE)[FCLEAN] Binary removed.$(NC)"

re: fclean all
