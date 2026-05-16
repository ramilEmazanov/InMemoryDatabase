add_library(memory STATIC memory_controller.cpp)
target_include_directories(memory PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
