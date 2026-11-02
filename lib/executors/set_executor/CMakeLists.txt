add_library(set_executor STATIC set_executor.cpp)
target_include_directories(set_executor
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(set_executor PUBLIC reply storage parser)
