add_library(geo_spatial STATIC geo_methods.cpp)
target_include_directories(geo_spatial PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
