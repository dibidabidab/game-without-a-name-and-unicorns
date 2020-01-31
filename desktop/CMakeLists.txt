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

# -no-pie is used to get 'application/x-application' as mime type instead of 'application/x-sharedlib'
set(CMAKE_CXX_FLAGS  "-no-pie -O3")

