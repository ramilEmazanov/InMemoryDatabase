add_library(geo_executor STATIC geo_executor.cpp)
target_include_directories(geo_executor
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(geo_executor PUBLIC reply storage parser geo_spatial)
