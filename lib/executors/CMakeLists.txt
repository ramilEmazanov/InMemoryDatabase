add_subdirectory(string_executor)
add_subdirectory(list_executor)
add_subdirectory(set_executor)
add_subdirectory(geo_executor)
add_subdirectory(general_executor)

add_library(executor STATIC command_executor.cpp)
target_include_directories(executor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(executor PUBLIC
  overload
  string_executor
  list_executor
  set_executor
  geo_executor
  general_executor
)
