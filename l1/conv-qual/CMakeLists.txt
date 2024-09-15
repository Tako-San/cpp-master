add_library(l1-tokenizing tokenizing.cc tokenizing.hh)
target_link_libraries(l1-tokenizing PUBLIC cpp-master-settings)

add_executable(l1-conv-qual conv-qual.cc)
target_link_libraries(l1-conv-qual PUBLIC cpp-master-settings l1-tokenizing)

# add_executable(l1-conv-qual-test conv-qual-test.cc)
# target_link_libraries(l1-conv-qual-test
#   PRIVATE
#     GTest::gtest_main
#     l1-tokenizing
# )

add_executable(l1-tokenizing-test tokenizing-test.cc)
target_link_libraries(l1-tokenizing-test
  PRIVATE
    GTest::gtest_main
    l1-tokenizing
)

include(GoogleTest)
gtest_discover_tests(l1-tokenizing-test)

