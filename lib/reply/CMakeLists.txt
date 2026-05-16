add_library(reply STATIC reply_formatter.cpp)
target_include_directories(reply PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(reply PUBLIC overload)
