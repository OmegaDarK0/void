EXEC      := void
VERSION   := 1.0

ARCH      ?= x86_64

SRC_DIR   := src
INC_DIR   := include

LIB_DIR   := lib/$(ARCH)
BIN_DIR   := bin/$(ARCH)

BUILD_DIR := build/$(ARCH)
OBJ_DIR   := $(BUILD_DIR)/obj
DEP_DIR   := $(BUILD_DIR)/dep

CC        := gcc
CXX       := g++

ifeq ($(ARCH),i386)
    ARCH_FLAGS := -m32
else
    ARCH_FLAGS := -m64
endif

CFLAGS    := $(ARCH_FLAGS) -std=c17 -Wall -Wextra -I$(INC_DIR) -DVERSION=\"$(VERSION)\"
CXXFLAGS  := $(ARCH_FLAGS) -std=c++20 -Wall -Wextra -I$(INC_DIR) -DVERSION=\"$(VERSION)\"

LDFLAGS   := $(ARCH_FLAGS) -static-libgcc -static-libstdc++ -L$(LIB_DIR) -Wl,--enable-new-dtags -Wl,-rpath='$$ORIGIN/../../$(LIB_DIR)'
LDLIBS    := -lSDL2 -lSDL2_image -lm -ldl -pthread

DEPFLAGS   = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

BUILD ?= release
ifeq ($(BUILD),debug)
    CFLAGS   += -Og -g -D_DEBUG
    CXXFLAGS += -Og -g -D_DEBUG
else
    CFLAGS   += -O2 -fstack-protector-strong -DNDEBUG
    CXXFLAGS += -O2 -fstack-protector-strong -DNDEBUG
    LDFLAGS  += -s -Wl,-z,relro,-z,now
endif

SRCS_C    := $(shell find $(SRC_DIR) -type f -name "*.c")
SRCS_CPP  := $(shell find $(SRC_DIR) -type f -name "*.cpp")

OBJS      := $(SRCS_C:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o) \
             $(SRCS_CPP:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

DEPS      := $(SRCS_C:$(SRC_DIR)/%.c=$(DEP_DIR)/%.d) \
             $(SRCS_CPP:$(SRC_DIR)/%.cpp=$(DEP_DIR)/%.d)

TARGET_BIN := $(BIN_DIR)/$(EXEC)

BLUE   := \033[0;34m
GREEN  := \033[0;32m
YELLOW := \033[0;33m
NC     := \033[0m

.PHONY: all clean fclean re run debug

all: $(TARGET_BIN)

$(TARGET_BIN): $(OBJS)
	@mkdir -p $(dir $@)
	@echo "$(BLUE)[LD] Linking: $@$(NC)"
	@$(CXX) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@

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
