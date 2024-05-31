BUILD_DIR  = ./build
TARGET_DIR = ./bin

LIBRARIES  = libglad.a libglfw3.a libsndfile.a libportaudio.a
LIBRARIES := $(addprefix $(TARGET_DIR)/,$(LIBRARIES))

.DELETE_ON_ERROR:
all: $(LIBRARIES)

PA_BUILD_OPTIONS = \
	-D PA_BUILD_SHARED_LIBRARY=OFF \
	-D PA_BUILD_TESTS=OFF \
	-D PA_BUILD_EXAMPLES=OFF
$(BUILD_DIR)/portaudio: ./portaudio
	cmake -S $< -B $@ $(PA_BUILD_OPTIONS) \
		-D CMAKE_ARCHIVE_OUTPUT_DIRECTORY='$(abspath $(TARGET_DIR))' \
		-D CMAKE_LIBRARY_OUTPUT_DIRECTORY='$(abspath $(TARGET_DIR))'
$(TARGET_DIR)/libportaudio.a: $(BUILD_DIR)/portaudio
	cmake --build $<

LIBSNDFILE_BUILD_OPTIONS = \
	-D BUILD_SHARED_LIBRARY=OFF \
	-D BUILD_PROGRAMS=OFF \
	-D BUILD_EXAMPLES=OFF \
	-D BUILD_TESTING=OFF \
	-D ENABLE_EXTERNAL_LIBS=ON \
	-D ENABLE_MPEG=ON \
	-D ENABLE_EXPERIMENTAL=OFF
$(BUILD_DIR)/libsndfile: ./libsndfile
	cmake -S $< -B $@ $(LIBSNDFILE_BUILD_OPTIONS) \
		-D CMAKE_ARCHIVE_OUTPUT_DIRECTORY='$(abspath $(TARGET_DIR))' \
		-D CMAKE_LIBRARY_OUTPUT_DIRECTORY='$(abspath $(TARGET_DIR))'
$(TARGET_DIR)/libsndfile.a: $(BUILD_DIR)/libsndfile
	cmake --build $<

GLFW_BUILD_OPTIONS = \
	-D BUILD_SHARED_LIBS=OFF \
	-D GLFW_BUILD_EXAMPLES=OFF \
	-D GLFW_BUILD_TESTS=OFF \
	-D GLFW_BUILD_DOCS=OFF \
	-D GLFW_INSTALL=OFF \
	-D GLFW_BUILD_WAYLAND=OFF
$(BUILD_DIR)/glfw: ./glfw
	cmake -S $< -B $@ $(GLFW_BUILD_OPTIONS) \
		-D CMAKE_ARCHIVE_OUTPUT_DIRECTORY='$(abspath $(TARGET_DIR))' \
		-D CMAKE_LIBRARY_OUTPUT_DIRECTORY='$(abspath $(TARGET_DIR))'
$(TARGET_DIR)/libglfw3.a: $(BUILD_DIR)/glfw
	cmake --build $<

$(TARGET_DIR)/libglad.a: ./glad
	@mkdir -p $(BUILD_DIR)/glad
	gcc -I$</include -c -o $(BUILD_DIR)/glad/glad.o $</src/glad.c
	ar rcs $@ $(BUILD_DIR)/glad/glad.o
	
.PHONY: clean
clean:
	-rm -r $(TARGET_DIR)
	-rm -r $(BUILD_DIR)
