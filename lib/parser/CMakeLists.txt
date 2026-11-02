add_subdirectory(lexer)

add_library(parser STATIC
  command_parser.cpp
  parser_commands_table.cpp
  commands_parsers/string_parser.cpp
  commands_parsers/general_parser.cpp
  commands_parsers/list_parser.cpp
  commands_parsers/set_parser.cpp
  commands_parsers/geo_parser.cpp
)
target_include_directories(parser PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/commands
  ${CMAKE_CURRENT_SOURCE_DIR}/commands_parsers
)
target_link_libraries(parser PUBLIC lexer errors)
