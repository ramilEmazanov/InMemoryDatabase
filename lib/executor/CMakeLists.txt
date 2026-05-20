add_library(executor STATIC command_executor.cpp)
target_include_directories(executor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(executor PUBLIC parser storage reply overload memory geo_spatial)
