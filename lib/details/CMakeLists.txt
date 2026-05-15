cmake_minimum_required(VERSION 3.23)

add_library(details)

target_sources(details 
PRIVATE
  commands_table.cpp
  parser.cpp
  storage.cpp
PUBLIC
  FILE_SET HEADERS
  FILES
    parser.hpp
    commands_table.hpp
    storage.hpp
    types.hpp
)

target_link_libraries(details
PUBLIC
PRIVATE
)