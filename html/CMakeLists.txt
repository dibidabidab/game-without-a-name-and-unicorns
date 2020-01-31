cmake_minimum_required(VERSION 3.0)

project(game)
set(EXECUTABLE_OUTPUT_PATH "out")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE source ../source/*)
add_executable(game ${source})
set_target_properties(game PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET game PROPERTY CXX_STANDARD 17)
set_property(TARGET game PROPERTY CXX_STANDARD_REQUIRED ON)

# gameutils repo:
add_subdirectory(../gu/library/ ./bin/gu)
target_link_libraries(game gameutils)

set(CMAKE_CXX_FLAGS  "-O3")

set_target_properties(game PROPERTIES SUFFIX ".html")
set_target_properties(game PROPERTIES LINK_FLAGS "-s WASM=1 -s USE_GLFW=3 -s USE_WEBGL2=1 -s BINARYEN_METHOD='native-wasm' --preload-file ../assets --shell-file index.html")

# creates the "out" directory:
add_custom_command(
    TARGET game PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E make_directory out)

# removes the old "out" directory:
add_custom_target(
    removeOut
    COMMAND ${CMAKE_COMMAND} -E remove_directory out)
add_dependencies(game removeOut)
