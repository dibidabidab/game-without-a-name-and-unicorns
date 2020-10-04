cmake_minimum_required(VERSION 3.0)

project(game)

add_compile_definitions(PUT_A_SOCK_IN_IT)
set(EXECUTABLE_OUTPUT_PATH "out")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PythonInterp 3.7 REQUIRED)
set(LUA_SERDE_COMMAND ${PYTHON_EXECUTABLE} ../external/dibidab-engine/external/lua-serde/lua-json-serde.py ../source/generated/ ../source/ ../../external/dibidab-engine/source/macro_magic/component.h)

execute_process(COMMAND ${LUA_SERDE_COMMAND} WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})


file(GLOB_RECURSE source ../source/*)
add_executable(game ${source})
set_target_properties(game PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET game PROPERTY CXX_STANDARD 17)
set_property(TARGET game PROPERTY CXX_STANDARD_REQUIRED ON)

# dibidab-engine repo:
add_subdirectory(../external/dibidab-engine ./bin/dibidab-engine)
target_link_libraries(game dibidab)


# ---Generate structs with LUA SERDE---

add_custom_target(
        generate_structs_for_game ALL
        COMMAND ${LUA_SERDE_COMMAND}
        COMMENT "Generating structs from yaml (game)"
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)
add_dependencies(game generate_structs_for_game)



# for debugging: set(CMAKE_CXX_FLAGS  "-O3 -g4 -lwebsocket.js -s ASSERTIONS=2 -s DEMANGLE_SUPPORT=1")
set(CMAKE_CXX_FLAGS  "-O3 -lwebsocket.js")

set_target_properties(game PROPERTIES SUFFIX ".html")
set_target_properties(game PROPERTIES LINK_FLAGS "-s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=0 -s WASM=1 -s USE_GLFW=3 -s USE_WEBGL2=1 -s BINARYEN_METHOD='native-wasm' --preload-file ../assets --shell-file index.html")

# creates the "out" directory:
add_custom_command(
    TARGET game PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E make_directory out)

# removes the old "out" directory:
add_custom_target(
    removeOut
    COMMAND ${CMAKE_COMMAND} -E remove_directory out)
add_dependencies(game removeOut)
