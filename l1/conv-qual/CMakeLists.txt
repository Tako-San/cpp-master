add_executable(l1-conv-qual conv-qual.cc)
target_link_libraries(l1-conv-qual PRIVATE cpp-master-settings)

add_executable(l1-tokenizing tokenizing.cc)
target_link_libraries(l1-tokenizing PRIVATE cpp-master-settings)

add_executable(l1-conv-qual-test conv-qual-test.cc)
target_link_libraries(l1-conv-qual-test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(l1-conv-qual-test)

