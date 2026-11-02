add_library(runner STATIC runner.cpp)
target_include_directories(runner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(runner PUBLIC executor reply)
