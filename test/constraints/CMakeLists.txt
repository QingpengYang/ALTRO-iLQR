add_executable(constraints_test
  constraints_test.cpp
)
target_link_libraries(constraints_test
  PRIVATE
  common
  constraints
  gtest_main
  problem
  utils
)
gtest_discover_tests(constraints_test)