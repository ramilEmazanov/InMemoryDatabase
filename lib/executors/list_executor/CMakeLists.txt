add_library(list_executor STATIC list_executor.cpp)
target_include_directories(list_executor
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(list_executor PUBLIC reply storage parser)
