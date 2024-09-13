add_executable(l1-streq-test streq-test.cc)
target_link_libraries(l1-streq-test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(l1-streq-test)
