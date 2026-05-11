cmake_minimum_required(VERSION 3.20)

project(GameEngine LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Important for IDEs (clangd, VSCode)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -----------------------
# Dependencies
# -----------------------
include(FetchContent)
find_package(OpenGL REQUIRED)

FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG 3.3.9
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
	json
	GIT_REPOSITORY https://github.com/nlohmann/json.git
	GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(json)

FetchContent_Declare(
	stb
	GIT_REPOSITORY https://github.com/nothings/stb.git
	GIT_TAG master
)

FetchContent_MakeAvailable(stb)

FetchContent_Declare(
	obj
	GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader
	GIT_TAG v2.0.0rc13
)

FetchContent_MakeAvailable(obj)

FetchContent_Declare(
  gltf
  GIT_REPOSITORY https://github.com/syoyo/tinygltf
  GIT_TAG v2.9.3
)

FetchContent_MakeAvailable(gltf)

FetchContent_Declare(
	imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG docking
)

FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
	${imgui_SOURCE_DIR}/imgui.cpp
	${imgui_SOURCE_DIR}/imgui_draw.cpp
	${imgui_SOURCE_DIR}/imgui_tables.cpp
	${imgui_SOURCE_DIR}/imgui_widgets.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
	${imgui_SOURCE_DIR}
	${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

add_executable(engine
	src/aabb.cpp
	src/camera.cpp
	src/config.cpp
	src/entry.cpp
	src/file.cpp
	src/globals.cpp
	src/keyboard.cpp
	src/mouse.cpp
	src/opengl-helpers.cpp
	src/ray.cpp
	src/render.cpp
	src/scene.cpp
	src/shader.cpp
	src/time.cpp
	src/tiny.cpp
	src/transform.cpp
	src/ui.cpp
)

target_link_libraries(engine PRIVATE
	OpenGL::GL
	glfw
	nlohmann_json::nlohmann_json
	glm::glm
	tinyobjloader
	tinygltf
	imgui
)

target_include_directories(engine PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${stb_SOURCE_DIR}
)

target_compile_definitions(engine PRIVATE GL_GLEXT_PROTOTYPES)

set_target_properties(engine PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist
)

add_custom_command(TARGET engine POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
    ${CMAKE_BINARY_DIR}/dist/shaders
	COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/dist/assets
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/config.json
    ${CMAKE_BINARY_DIR}/dist/config.json
  COMMENT "Copying shaders and config..."
)

# -----------------------
# Subdirectories
# -----------------------
add_subdirectory(tests/opengl-test)
add_subdirectory(tests/quad-test)
add_subdirectory(tests/texture-test)
add_subdirectory(tests/transformation-test)
add_subdirectory(tests/cube-test)
add_subdirectory(tests/obj-test)
add_subdirectory(tests/obj-texture-test)
add_subdirectory(tests/lighting-test)
