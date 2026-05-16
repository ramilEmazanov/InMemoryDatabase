add_library(lexer STATIC tokenizer.cpp)
target_include_directories(lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lexer PUBLIC errors)
