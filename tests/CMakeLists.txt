add_executable(labwork10_tests
  string_tests.cpp
  list_tests.cpp
  set_tests.cpp
)

target_link_libraries(labwork10_tests PRIVATE
  GTest::gtest_main
  GTest::gmock
  runner
)

target_include_directories(labwork10_tests PRIVATE
  ${PROJECT_SOURCE_DIR}
)

include(GoogleTest)
gtest_discover_tests(labwork10_tests)