add_library(general_executor STATIC general_executor.cpp)
target_include_directories(general_executor
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(general_executor PUBLIC reply storage parser memory)
