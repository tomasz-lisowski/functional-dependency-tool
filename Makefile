CC = gcc
CFLAGS	+= 	-g -std=c11 -m32 \
			-Wall -Wextra -Wpedantic \
			-l m \
# CFLAGS += -Werror

SRC_DIR = src
OUT_DIR = build
OUT_NAME = func_deps_tool

SRC_EXT = c
SRCS = $(wildcard $(SRC_DIR)/*.$(SRC_EXT))
OBJS = $(patsubst $(SRC_DIR)/%.$(SRC_EXT),$(OUT_DIR)/%.o,$(SRCS))
OUT_EXT_WIN = exe
OUT_EXT_LNX = bin

# Detect the OS being used
ifeq ($(OS),Windows_NT) 
	OS_DETECTED := Windows
else
	OS_DETECTED := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif
# Determine the right executable extension and exit if platform is unsupported
ifeq ($(OS_DETECTED),Windows)
	OUT_NAME := $(OUT_NAME).$(OUT_EXT_WIN)
else 
ifeq ($(OS_DETECTED),Linux)
	OUT_NAME := $(OUT_NAME).$(OUT_EXT_LNX)
else
	$(error This platform is not yet supported)
	exit 1;
endif
endif

.PHONY: all
all: dir_out $(OUT_DIR)/$(OUT_NAME)

.PHONY: dir_out
dir_out:
ifeq ($(OS_DETECTED),Windows)
	@if not exist $(OUT_DIR) mkdir $(OUT_DIR)
endif
ifeq ($(OS_DETECTED),Linux)
	@mkdir -p $(OUT_DIR)
endif

$(OUT_DIR)/$(OUT_NAME): $(OBJS)
	$(CC) $^ -o $@ $(CFLAGS)

$(OBJS): $(OUT_DIR)/%.o : $(SRC_DIR)/%.$(SRC_EXT)
	$(CC) $< -o $@ -c $(CFLAGS)

.PHONY: clean
clean:
ifeq ($(OS_DETECTED),Windows)
	@if exist $(OUT_DIR) rmdir /q /s $(OUT_DIR)
endif
ifeq ($(OS_DETECTED),Linux)
	@rm -rf $(OUT_DIR)
endif

.PHONY: run
run: all
ifeq ($(OS_DETECTED),Windows)
	@echo.
endif
ifeq ($(OS_DETECTED),Linux)
	@echo -e "\n"
endif
	@$(OUT_DIR)/$(OUT_NAME)
