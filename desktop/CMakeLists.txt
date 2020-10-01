cmake_minimum_required(VERSION 3.0)

if (MSVC)
    # place game.exe and OpenAL32.dll in same folder:
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif ()

project(game)


# define GIT_COMMIT_HASH and GIT_COMMIT_MESSAGE. Todo: generate a .h file instead, since that won't cause everything to recompile
execute_process(COMMAND git log -1 --pretty=%h OUTPUT_VARIABLE GIT_COMMIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git log -1 --pretty=%s OUTPUT_VARIABLE GIT_COMMIT_MESSAGE OUTPUT_STRIP_TRAILING_WHITESPACE)
add_compile_definitions(GIT_COMMIT_HASH="${GIT_COMMIT_HASH}")
add_compile_definitions(GIT_COMMIT_MESSAGE="${GIT_COMMIT_MESSAGE}")



set(EXECUTABLE_OUTPUT_PATH "out")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

file(GLOB_RECURSE source ../source/*)
add_executable(game ${source})
set_target_properties(game PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET game PROPERTY CXX_STANDARD 17)
set_property(TARGET game PROPERTY CXX_STANDARD_REQUIRED ON)

# dibidab-engine repo:
add_subdirectory(../external/dibidab-engine ./bin/dibidab-engine)
target_link_libraries(game dibidab)


# ---Generate structs with LUA SERDE---
find_package(PythonInterp 3.7 REQUIRED)

add_custom_target(
        generate_structs_for_game ALL
        COMMAND ${PYTHON_EXECUTABLE} ../external/dibidab-engine/external/lua-serde/lua-json-serde.py ../source/generated/ ../source/
        COMMENT "Generating structs from yaml (game)"
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)

add_dependencies(game generate_structs_for_game)



if (MSVC)
    set(CMAKE_CXX_FLAGS  "/Ox /EHsc /bigobj")

else()
    # -no-pie is used to get 'application/x-application' as mime type instead of 'application/x-sharedlib'
    set(CMAKE_CXX_FLAGS  "-no-pie")

    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
    endif()
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")

endif()
