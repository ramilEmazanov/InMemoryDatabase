add_library(storage STATIC storage.cpp types.cpp)
target_include_directories(storage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(storage PUBLIC memory errors time_controller)
