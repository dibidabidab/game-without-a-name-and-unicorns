cmake_minimum_required(VERSION 3.0)

project(game)

add_compile_definitions(PUT_A_SOCK_IN_IT)
set(EXECUTABLE_OUTPUT_PATH "out")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE source ../source/*)
add_executable(game ${source})
set_target_properties(game PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET game PROPERTY CXX_STANDARD 17)
set_property(TARGET game PROPERTY CXX_STANDARD_REQUIRED ON)

# gameutils repo:
add_subdirectory(../external/gu/library/ ./bin/gu)
target_link_libraries(game gameutils)


# ---ASIO---
include_directories(../external/asio/asio/include)
target_include_directories(game PUBLIC ../external/asio/asio/include)


# ---LUA---
add_subdirectory(../external/lua ./bin/lua)
target_link_libraries(game lua)


# for debugging: set(CMAKE_CXX_FLAGS  "-O3 -g4 -lwebsocket.js -s ASSERTIONS=2 -s DEMANGLE_SUPPORT=1")
set(CMAKE_CXX_FLAGS  "-O3 -lwebsocket.js")

set_target_properties(game PROPERTIES SUFFIX ".html")
set_target_properties(game PROPERTIES LINK_FLAGS "-s DISABLE_EXCEPTION_CATCHING=0 -s WASM=1 -s USE_GLFW=3 -s USE_WEBGL2=1 -s BINARYEN_METHOD='native-wasm' --preload-file ../assets --shell-file index.html")

# creates the "out" directory:
add_custom_command(
    TARGET game PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E make_directory out)

# removes the old "out" directory:
add_custom_target(
    removeOut
    COMMAND ${CMAKE_COMMAND} -E remove_directory out)
add_dependencies(game removeOut)
