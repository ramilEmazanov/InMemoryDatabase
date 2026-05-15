add_library(handlers)

target_sources(handlers
PRIVATE
  string_cmd.cpp
PUBLIC
  FILE_SET HEADERS
  FILES
    string_cmd.hpp
)

target_link_libraries(handlers
PUBLIC
details
PRIVATE
)