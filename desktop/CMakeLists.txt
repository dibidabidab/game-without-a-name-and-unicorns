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

file(GLOB_RECURSE source ../source/*)
add_executable(game ${source})
set_target_properties(game PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET game PROPERTY CXX_STANDARD 17)
set_property(TARGET game PROPERTY CXX_STANDARD_REQUIRED ON)

# gameutils repo:
add_subdirectory(../gu/library/ ./bin/gu)
target_link_libraries(game gameutils)


# ---WEBSOCKETPP---
include_directories(../websocketpp)
target_include_directories(game PUBLIC ../websocketpp)


# ---ASIO---
add_definitions (-D_WEBSOCKETPP_CPP11_STL_)
include_directories(../asio/asio/include)
target_include_directories(game PUBLIC ../asio/asio/include)

if (MSVC)
    set(CMAKE_CXX_FLAGS  "/Ox /EHsc")

else()
    # -no-pie is used to get 'application/x-application' as mime type instead of 'application/x-sharedlib'
    set(CMAKE_CXX_FLAGS  "-no-pie -O3")

endif()
