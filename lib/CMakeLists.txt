cmake_minimum_required(VERSION 3.23)

add_library(overload INTERFACE)
target_include_directories(overload INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(errors)
add_subdirectory(memory)
add_subdirectory(time_controller)
add_subdirectory(storage)
add_subdirectory(parser)
add_subdirectory(reply)
add_subdirectory(executor)
add_subdirectory(runner)
