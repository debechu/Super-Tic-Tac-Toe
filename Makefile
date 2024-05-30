BUILD_DIR = ./build

TARGET_DIR  = ./bin
TARGET_EXEC = super-tic-tac-toe

INCLUDE_DIRS   = ./include ./deps/glad/include ./deps/glfw/include
INCLUDE_FLAGS := $(addprefix -I,$(INCLUDE_DIRS))

SOURCE_DIR    = ./src
SOURCE_FILES := $(shell find $(SOURCE_DIR) -name '*.c')
OBJECT_FILES := $(SOURCE_FILES:$(SOURCE_DIR)/%.c=$(BUILD_DIR)/%.o)

LIBRARIES  = libglad.a libglfw3.a
LIB_FILES := $(addprefix ./deps/bin/,$(LIBRARIES))
LIB_FLAGS := $(addprefix -l:,$(LIBRARIES)) -lm

.DELETE_ON_ERROR:
$(TARGET_DIR)/$(TARGET_EXEC): $(LIB_FILES) $(OBJECT_FILES)
	@mkdir -p $(dir $@)
	gcc -L./deps/bin -o $@ $(OBJECT_FILES) $(LIB_FLAGS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	gcc $(INCLUDE_FLAGS) -c -o $@ $<

# Pattern rules don't automatically search for matching
# files, so we have to manually specify them.
$(SOURCE_FILES):

$(LIB_FILES):
	cd ./deps && $(MAKE) ./bin/$(notdir $@)

.PHONY: clean
clean:
	-rm -r $(BUILD_DIR)
	-rm -r $(TARGET_DIR)

	-cd ./deps && $(MAKE) clean
