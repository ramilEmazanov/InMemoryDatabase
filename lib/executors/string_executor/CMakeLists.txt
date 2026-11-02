add_library(string_executor STATIC string_executor.cpp)
target_include_directories(string_executor
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(string_executor PUBLIC reply storage parser)
