NAME      := void
VERSION   := 0.1.0-alpha

ifeq ($(OS),Windows_NT)
    HOST_OS   := windows
    HOST_ARCH := $(PROCESSOR_ARCHITECTURE)

    RED    :=
    GREEN  :=
    YELLOW :=
    BLUE   :=
    NC     :=
else
    RAW_OS   := $(shell uname -s 2>/dev/null || echo Unknown)
    RAW_ARCH := $(shell uname -m 2>/dev/null || echo Unknown)

    ifeq ($(RAW_OS),Darwin)
        $(error "Darwin is not yet implemented.")
    else ifeq ($(RAW_OS),Linux)
        HOST_OS := linux
    else
        $(error "Unsupported OS: $(RAW_OS)")
    endif

    ifeq ($(RAW_ARCH),aarch64)
        $(error "arm64 is not yet implemented.")
    else ifeq ($(RAW_ARCH),arm64)
        $(error "arm64 is not yet implemented.")
    else ifeq ($(RAW_ARCH),amd64)
        HOST_ARCH := x86_64
    else ifeq ($(RAW_ARCH),x86_64)
        HOST_ARCH := x86_64
    else ifeq ($(RAW_ARCH),x86)
        HOST_ARCH := x86
    else ifneq (,$(filter i%86,$(RAW_ARCH)))
        HOST_ARCH := x86
    else
        $(error "Unsupported Architecture: $(RAW_ARCH)")
    endif

    RED    := \033[0;31m
    GREEN  := \033[0;32m
    YELLOW := \033[0;33m
    BLUE   := \033[0;34m
    NC     := \033[0m
endif

OS   ?= $(HOST_OS)
ARCH ?= x86_64

TARGET_OS   := $(OS)
TARGET_ARCH := $(ARCH)

CC       := gcc
CXX      := g++
AR       := ar
BIN_EXT  :=

ifeq ($(TARGET_OS),windows)
    BIN_EXT := .exe
    LDLIBS   := -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows
    OS_CFLAGS  := -Dmain=SDL_main
    OS_LDFLAGS :=

    RUNTIME ?= msvcrt

    ifneq ($(HOST_OS),windows)
        ifeq ($(TARGET_ARCH),x86_64)
            ifeq ($(RUNTIME),ucrt)
                CC  := x86_64-w64-mingw32ucrt-gcc
                CXX := x86_64-w64-mingw32ucrt-g++
                AR  := x86_64-w64-mingw32ucrt-ar
            else
                CC  := x86_64-w64-mingw32-gcc
                CXX := x86_64-w64-mingw32-g++
                AR  := x86_64-w64-mingw32-ar
            endif
        else
            CC  := i686-w64-mingw32-gcc
            CXX := i686-w64-mingw32-g++
            AR  := i686-w64-mingw32-ar
        endif
    endif
else ifeq ($(TARGET_OS),linux)
    BIN_EXT  :=
    LDLIBS   := -lSDL2 -lSDL2_image -lm -ldl -pthread
    OS_CFLAGS  := -fPIC
    OS_LDFLAGS := -pie -Wl,--enable-new-dtags -Wl,-rpath='$$ORIGIN'
endif

ifeq ($(TARGET_ARCH),x86)
    ARCH_FLAGS := -m32
else
    ARCH_FLAGS := -m64
endif

BIN_NAME   := $(NAME)$(BIN_EXT)
LIB_NAME   := lib$(NAME).a

SRC_DIR   := src
INC_DIR   := include

LIB_DIR   := lib/$(TARGET_OS)-$(TARGET_ARCH)
BIN_DIR   := bin/$(TARGET_OS)-$(TARGET_ARCH)
BUILD_DIR := build/$(TARGET_OS)-$(TARGET_ARCH)

OBJ_DIR   := $(BUILD_DIR)/obj
DEP_DIR   := $(BUILD_DIR)/dep

TARGET_BIN := $(BIN_DIR)/$(BIN_NAME)
TARGET_LIB := $(LIB_DIR)/$(LIB_NAME)

COMMON_FLAGS := $(ARCH_FLAGS) -Wall -Wextra -I$(INC_DIR) -DVERSION=\"$(VERSION)\" $(OS_CFLAGS)
LDFLAGS      := $(ARCH_FLAGS) -static-libgcc -static-libstdc++ -L$(LIB_DIR) $(OS_LDFLAGS)
DEPFLAGS      = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

BUILD ?= release
ifeq ($(BUILD),debug)
    COMMON_FLAGS += -Og -g -D_DEBUG
else
    COMMON_FLAGS += -O2 -fstack-protector-strong -DNDEBUG
    LDFLAGS      += -s
    ifeq ($(TARGET_OS),linux)
        LDFLAGS  += -Wl,-z,relro,-z,now
    endif
endif

CFLAGS       := $(COMMON_FLAGS) -std=c17
CXXFLAGS     := $(COMMON_FLAGS) -std=c++20

SRC_MAIN := $(wildcard $(SRC_DIR)/main.c) $(wildcard $(SRC_DIR)/main.cpp)

OBJ_MAIN := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(filter %.c, $(SRC_MAIN))) \
            $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(filter %.cpp, $(SRC_MAIN)))

SRCS_C   := $(filter-out $(SRC_MAIN), $(shell find $(SRC_DIR) -type f -name "*.c"))
SRCS_CPP := $(filter-out $(SRC_MAIN), $(shell find $(SRC_DIR) -type f -name "*.cpp"))

OBJS_CORE := $(SRCS_C:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o) \
             $(SRCS_CPP:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

DEPS      := $(OBJS_CORE:%.o=%.d) $(OBJ_MAIN:%.o=%.d)

EXEC := $(TARGET_BIN)

ifeq ($(TARGET_OS),windows)
    ifneq ($(HOST_OS),windows)
        EXEC := WINEDEBUG=-all wine $(TARGET_BIN)
    endif
endif

define POST_BUILD_STEP
	@if [ "$(TARGET_OS)" = "windows" ]; then \
		echo "$(BLUE)[DLL] Copying dynamic libraries...$(NC)"; \
		cp $(LIB_DIR)/*.dll $(BIN_DIR)/ 2>/dev/null || true; \
	elif [ "$(TARGET_OS)" = "linux" ]; then \
		echo "$(BLUE)[SO] Copying dynamic libraries...$(NC)"; \
		cp -P $(LIB_DIR)/*.so* $(BIN_DIR)/ 2>/dev/null || true; \
	fi
endef

.PHONY: all lib clean fclean distclean re run debug info

all: $(TARGET_BIN)

lib: $(TARGET_LIB)

$(OBJ_DIR) $(DEP_DIR) $(BIN_DIR) $(LIB_DIR):
	@mkdir -p $@

$(TARGET_BIN): $(OBJ_MAIN) $(TARGET_LIB) | $(BIN_DIR)
	@echo "$(BLUE)[LD] Linking: $@$(NC)"
	@$(CXX) $(OBJ_MAIN) $(TARGET_LIB) $(LDFLAGS) $(LDLIBS) -o $@
	$(POST_BUILD_STEP)

$(TARGET_LIB): $(OBJS_CORE) | $(LIB_DIR)
	@echo "$(BLUE)[AR] Archiving: $@$(NC)"
	@$(AR) rcs $@ $(OBJS_CORE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR) $(DEP_DIR)
	@echo "$(YELLOW)[CC] $<$(NC)"
	@$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) $(DEP_DIR)
	@echo "$(YELLOW)[CXX] $<$(NC)"
	@$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

-include $(DEPS)

run: all
	@echo "$(GREEN)[RUN] Launching...$(NC)"
	@$(EXEC) $(ARGS)

debug:
	@$(MAKE) --no-print-directory all BUILD=debug
	@gdb $(TARGET_BIN)

clean:
	@rm -rf $(BUILD_DIR)
	@echo "$(RED)[CLEAN] Build directory removed.$(NC)"

fclean: clean
	@rm -rf $(TARGET_BIN) $(TARGET_LIB)
	@echo "$(RED)[FCLEAN] Binary removed.$(NC)"

distclean:
	@rm -rf build bin
	@rm -f lib/*/$(LIB_NAME)
	@echo "$(RED)[DISTCLEAN] All builds and binaries removed.$(NC)"

re:
	@$(MAKE) --no-print-directory fclean
	@$(MAKE) --no-print-directory all

info:
	@echo "$(BLUE)==================================================$(NC)"
	@echo "$(GREEN)          VOID ENGINE - BUILD CONFIGURATION       $(NC)"
	@echo "$(BLUE)==================================================$(NC)"
	@echo "$(YELLOW)[Project]$(NC)"
	@echo "  Name     : $(NAME)"
	@echo "  Version  : $(VERSION)"
	@echo "  Build    : $(BUILD)"
	@echo ""
	@echo "$(YELLOW)[Environment]$(NC)"
	@echo "  Host OS  : $(HOST_OS) ($(HOST_ARCH))"
	@echo "  Target   : $(TARGET_OS) ($(TARGET_ARCH))"
	@echo ""
	@echo "$(YELLOW)[Toolchain]$(NC)"
	@echo "  C Compiler   : $(CC)"
	@echo "  C++ Compiler : $(CXX)"
	@echo "  Archiver     : $(AR)"
	@echo ""
	@echo "$(YELLOW)[Directories]$(NC)"
	@echo "  Sources  : $(SRC_DIR)/"
	@echo "  Includes : $(INC_DIR)/"
	@echo "  Objects  : $(OBJ_DIR)/"
	@echo "  Library  : $(LIB_DIR)/"
	@echo "  Binary   : $(BIN_DIR)/"
	@echo ""
	@echo "$(YELLOW)[Flags]$(NC)"
	@echo "  CFLAGS   : $(CFLAGS)"
	@echo "  CXXFLAGS : $(CXXFLAGS)"
	@echo "  LDFLAGS  : $(LDFLAGS)"
	@echo "  LDLIBS   : $(LDLIBS)"
	@echo "$(BLUE)==================================================$(NC)"