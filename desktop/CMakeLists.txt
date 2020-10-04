cmake_minimum_required(VERSION 3.0)

if (MSVC)
    # place game.exe and OpenAL32.dll in same folder:
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif ()

project(game)


set(EXECUTABLE_OUTPUT_PATH "out")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

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
