add_library(commands)

target_sources(commands
PRIVATE
  string_cmd.cpp
  control.cpp
PUBLIC
  FILE_SET HEADERS
  FILES
    string_cmd.hpp
    control.hpp
)

target_link_libraries(commands
PUBLIC
details
PRIVATE
)