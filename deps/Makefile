BUILD_DIR  = ./build
TARGET_DIR = ./bin

LIBRARIES  = libglad.a libglfw3.a
LIBRARIES := $(addprefix $(TARGET_DIR)/,$(LIBRARIES))

.DELETE_ON_ERROR:
all: $(LIBRARIES)

$(TARGET_DIR)/libglfw3.a: $(BUILD_DIR)/glfw
	cmake --build $(BUILD_DIR)/glfw

GLFW_BUILD_OPTIONS = \
	-D BUILD_SHARED_LIBS=OFF \
	-D GLFW_BUILD_EXAMPLES=OFF \
	-D GLFW_BUILD_TESTS=OFF \
	-D GLFW_BUILD_DOCS=OFF \
	-D GLFW_INSTALL=OFF \
	-D GLFW_BUILD_WAYLAND=OFF
$(BUILD_DIR)/glfw: ./glfw
	cmake -S ./glfw -B $@ $(GLFW_BUILD_OPTIONS) \
		-D CMAKE_ARCHIVE_OUTPUT_DIRECTORY='$(abspath $(TARGET_DIR))'
		-D CMAKE_LIBRARY_OUTPUT_DIRECTORY='$(abspath $(TARGET_DIR))'

$(TARGET_DIR)/libglad.a: ./glad
	@mkdir -p $(BUILD_DIR)/glad
	gcc -I./glad/include -c -o $(BUILD_DIR)/glad/glad.o ./glad/src/glad.c
	ar rcs $@ $(BUILD_DIR)/glad/glad.o
	
.PHONY: clean
clean:
	-rm -r $(TARGET_DIR)
	-rm -r $(BUILD_DIR)
